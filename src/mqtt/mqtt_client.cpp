#include "mqtt_client.h"

#include "./system/definitions.h"
#include "./system/system_configuration.h"
#include "./fs/sys_cfg.h"
#include "./mqtt/mqtt_remoteControl.h"
#include "./mqtt/mqtt_subscription.h"
#include "./mqtt/mqtt_dataStatus.h"
#include "./mqtt/mqtt_dataDeviceBatch.h"
#include "./fs/sys_logs_data.h"
#include "./hmi/led_task.h"
#include "./ota/ota_module.h"
#include "./wifi/wifi_module.h"
#include "./system/system_configuration.h"
#include "./system/system_json.h"
#include "./system/system_tasks.h"
#include "./system/utils.h"
#include "./configurator/configurator.h"



/**
 * MQTT status
 */
struct sMqttStat
{
    bool mqttConnected;
};


/**
 * Structure for MQTT configuration
 */
struct sMqttConfig
{
    char *mqttServer;
    char *mqttUser;
    char *mqttPassword;
    int port;
    bool secureConnection;
    MQTT_CALLBACK_SIGNATURE;
    std::function<void(char*, uint8_t*, unsigned int)> user_callback;
};



/* MQTT status */
struct sMqttStat mqttStat;
/* MQTT configuration */
struct sMqttConfig mqttConfig
{
    "mqtt.iotaap.io",   /* mqttServer              */
    "",                 /* mqttUser                */
    "",                 /* mqttPassword            */
    8883,               /* port                    */
    false,              /* secureConnection        */
    NULL,               /* MQTT_CALLBACK_SIGNATURE */
    NULL                /* user_callback           */
};

/* Data from JSON in structure */
struct sJsonKeys JsonMqttData[] = 
{
    {&mqttConfig.mqttServer      , JsonDataTypeString, "mqtt_server", "MQTT Server"      , NULL},
    {&mqttConfig.mqttUser        , JsonDataTypeString, "mqtt_user"  , "MQTT User"        , NULL},
    {&mqttConfig.mqttPassword    , JsonDataTypePass  , "mqtt_pass"  , "MQTT Password"    , NULL},
    {&mqttConfig.port            , JsonDataTypeInt   , "mqtt_port"  , "MQTT Port"        , NULL},
    {&mqttConfig.secureConnection, JsonDataTypeBool  , "secure_conn", "Secure Connection", "SCR"}
};

/* Queue for MQTT messaging */
LinkedList<sMqttContainer*> *mqttDataMsgs;
/* Topic list */
list <char*> mqttTopics;

/* Secure connection - EXTERN - shared between MQTT and OTA update */
WiFiClient *wifiClient;
/* MQTT uses secure network (shared with OTA) */
PubSubClient *_mqttClient;


bool InitMqttDone = true;
bool blockPublish = false;

/* Task */
static void MqttTask( void *parameter);
/* Add msg to list */
static void mqttPub( const char *topic, struct sMqttData *mqttData);
/* Physically send message */
static void publishMqttMessages( void);

/******************************************************************************/
/***** GLOBAL FUNCTIONS *******************************************************/
/******************************************************************************/

/**
 * @brief Create MqttProcess task
 * 
 */
static TaskHandle_t MqttHandler = NULL;
void connectToMqtt()
{
    if (IsConfiguratorActive())
    {
        return;
    }

    mqttDataMsgs  = new LinkedList<sMqttContainer*>();

    xTaskCreate(
        MqttTask,
        "MqttProcess",
        12000,
        NULL,
        1,
        &MqttHandler);
}


/**
 * @brief Save data from JSON document to local structure
 */
void InitMqttconfigDataFromJsonDocument( DynamicJsonDocument ConfigJson)
{
    int StructSize = sizeof(JsonMqttData)/sizeof(*JsonMqttData);
    InitDataFromSystemJson( ConfigJson, JsonMqttData, StructSize);
}

/**
 * @brief Stop publishing messages
 */
void stopPublishing( void)
{
    blockPublish = true;
}

/**
 * @brief Adds new message to queue, it will be picked up by the MqttProcess task and published to topic
 * 
 * @param topic MQTT topic
 * @param payload MQTT topic payload
 * @param retain Retain last message
 */
void mqttPublish(const char *topic, const char *name, const char *value, boolean retain)
{
    if (IsConfiguratorActive() || blockPublish)
    {
        return;
    }

    sMqttData *mqttData = new sMqttData;

    mqttData->name = new char[strlen(name)+1];
    strcpy( mqttData->name, name);

    mqttData->text = new char[strlen(value)+1];
    strcpy( mqttData->text, value);

    mqttData->flags = (retain << 0) | (1 << 1) | (0 << 2);
    mqttData->unix_ms = getUnixTimeMs();

    mqttPub( topic, mqttData);
}
void mqttPublish(const char *topic, const char *name, float value, boolean retain)
{
    if (IsConfiguratorActive() || blockPublish)
    {
        return;
    }

    sMqttData *mqttData = new sMqttData;

    mqttData->name = new char[strlen(name)+1];
    strcpy( mqttData->name, name);

    mqttData->value = value;
    mqttData->flags = (retain << 0) | (0 << 1) | (0 << 2);
    mqttData->unix_ms = getUnixTimeMs();
    
    mqttPub( topic, mqttData);
}
void mqttPublish(const char *topic, const char *payload, boolean retain)
{
    if (IsConfiguratorActive() || blockPublish)
    {
        return;
    }

    sMqttData *mqttData = new sMqttData;
    mqttData->name = 0;

    mqttData->payload = new char[strlen(payload)+1];
    strcpy( mqttData->payload, payload);

    mqttData->flags = (retain << 0) | (0 << 1) | (1 << 2);
    mqttData->unix_ms = getUnixTimeMs();
    
    mqttPub( topic, mqttData);
}

/******************************************************************************/
/***** DATA FUNCTIONS *********************************************************/
/******************************************************************************/

/**
 * @brief Check if MQTT is connected
 */
bool MqttIsConnected( void)
{
    return mqttStat.mqttConnected;
}

/**
 * @brief Set MQTT callback function
 */
void MqttSetCallback( MQTT_CALLBACK_SIGNATURE)
{
    mqttConfig.callback = MqttCallback;
    mqttConfig.user_callback = callback;
}
/**
 * @brief Returns MQTT user
 */
const char *MqttGetUser( void)
{
    return mqttConfig.mqttUser;
}
/**
 * @brief Runs Callback Function 
 * 
 * @param Topic MQTT Topic
 * @param Message MQTT Message
 * @param Length MQTT Message Length
 */
void MqttRunUserCallback( char *topic, uint8_t *message, unsigned int length)
{
    return mqttConfig.user_callback( topic, message, length);
}

/******************************************************************************/
/***** STATIC FUNCTIONS *******************************************************/
/******************************************************************************/
/**
 * @brief Task that takes care of the MQTT connectivity and operation
 * 
 * @param parameter 
 */
static void MqttTask(void *parameter)
{
    wifiClient = (mqttConfig.secureConnection) ? new WiFiClientSecure() : new WiFiClient();
    _mqttClient = new PubSubClient(*wifiClient);

    mqttSubscribeToRemoteControlTopics(); 

    mqttStat.mqttConnected = false; 
    while (1)
    {
        PRINT_EXTRA_STACK_IN_TASK();
        if (WifiIsConnected())
        {
            //Initalises security certificate
            wifiClient->setTimeout(12); // timeout argument is defined in seconds for setTimeout
            if (mqttConfig.secureConnection)
            {
                ((WiFiClientSecure*)wifiClient)->setCACert(SystemGetCAcertificate());
            }
            _mqttClient->setBufferSize(1024);

            /* Check if server is IP address */
            IPAddress srvAddr;
            if (srvAddr.fromString(mqttConfig.mqttServer))
            {
                _mqttClient->setServer(srvAddr, mqttConfig.port);
            }
            else
            {
                _mqttClient->setServer(mqttConfig.mqttServer, mqttConfig.port);
            }
            _mqttClient->setCallback(mqttConfig.callback);
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
    }
    _mqttClient->disconnect();
    mqttStat.mqttConnected = false;
    while (1)
    {
        PRINT_EXTRA_STACK_IN_TASK();
        while (WifiIsConnected())
        {
            PRINT_EXTRA_STACK_IN_TASK();
            mqttStat.mqttConnected = _mqttClient->connected();
            if (!mqttStat.mqttConnected)
            {
                systemLog(tWARNING, "Not connected to MQTT broker");
                vTaskDelay(1000 / portTICK_PERIOD_MS); // Re-attempts to connect every second
                _mqttClient->connect(SystemGetDeviceId(), mqttConfig.mqttUser, mqttConfig.mqttPassword);
                LedBlinkFast(); // Sets LED operating into fast mode - indicates not connected
                systemLog(tINFO, "Restoring subscribed topics");
                restoreSubscribedTopics();
            }
            else
            {
                _mqttClient->loop();
                publishSystemStatus();
                subscribeToTopics();
                unsubscribeFromTopics();
                publishMqttMessages();

                /* TODO
                * Updates are handled inside of this task due to System reset bug (due to two WiFiClientSecure instances used before)
                * this is a temporary workaround and should be improved later. 
                * How it works? 
                * System is using single WiFiClientSecure instance for both MQTTS and HTTPS. When update timing expires WiFiClientSecure instance
                * will be stopped and WiFiClientSecure instance will be used by HTTPS to check and do update.
                * After checking for update WiFiClientSecure will be stopped and MQTT Client will automatically connect
                * back to the MQTT Broker. 
                * Drawbacks of this solutions are that MQTT communication will not work during checking for update or updating. During update check, this is
                * only a short period of time, but during update it could take some time. During any of this two processes, data will be stored to queue, and
                * then MQTT connection will be detected as broken so data will be saved locally, and once MQTT connection is established data will be published. 
                * 
                * Ideal solution for this issue should be parallel MQTTS and OTA connection, but currently it's not possible to use two WiFiClientSecure instances
                * in parallel due to heap issues (consumes large amount of memory) and causes crashes.
                * 
                * Update: second instance of WiFiClientSecure has been removed as both MQTTS and OTA are now using the same certificate, OTA cert change code has been removed.
                * 
                * 
                */
                handleUpdates();
                //

                LedBlinkPeriodic(); // Sets LED operating into periodic mode - indicates connected
            }
            vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
        }
        _mqttClient->disconnect();
        mqttStat.mqttConnected = false;     // If wifi is not connected then mqtt is not connected also
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
    }
}


/**
 * @brief Add msg to publish list
 */
static void mqttPub(const char *topic, struct sMqttData *mqttData)
{
    /* Check if topic already exist */
    // int TopicList = 0;
    std::list<char*>::iterator it;
    for (it=mqttTopics.begin(); it!=mqttTopics.end(); ++it)
    {
        if (!strcmp(topic, *it))
        {
            break;
        }
    }

    /* Create message */
    sMqttContainer *newMessage = new sMqttContainer;

    /* Add new topic to list */
    if (it == mqttTopics.end())
    {
        char *newTopicInList = new char[strlen(topic)+1];
        strcpy( newTopicInList, topic);
        mqttTopics.push_back( newTopicInList);

        newMessage->topic = newTopicInList;
    }
    /* Topic already exist in list */
    else
    {
        newMessage->topic = *it;
    }

    newMessage->data = mqttData;

    /* Add full msg (topic+data) to list */
    mqttDataMsgs->add( newMessage);

}


/**
 * @brief Publishes MQTT messages from the queue
 */
static void publishMqttMessages( void)
{
    if (blockPublish)
    {
        return;
    }

    static unsigned long LastBatchPubTime = 0;
    unsigned long TimeNow = millis();

    /* Calculate number of data in non-batch list */
    int NumberOfMsgs = mqttDataMsgs->size();

    /* No data to send */
    if (!NumberOfMsgs &&
        (!BatchNumOfElements() || TimeNow-LastBatchPubTime < getBatchDiffPeriod()))
    {
        return;
    }

    /* Choose what to send */
    bool batchFiller = false;
    if (NumberOfMsgs < 10 &&
        NumberOfMsgs*2 < BatchFullnessPercent())
    {
        batchFiller = true;
    }


    /* SEND NON-BATCH DATA */
    if (!batchFiller)
    {
        struct sMqttContainer *cont = mqttDataMsgs->shift();
        struct sMqttData *data = cont->data;
        char *topic = cont->topic;
        bool retain = (data->flags << 1) & 0x01;

        /* SEND DATA */
        if (data->flags & (1<<2))
        {
            char *payload = data->payload;
            char logData[256];
            sprintf( logData, "Publishing message to topic: %s", topic);
            systemLog(tINFO, logData);

            /* Publish */
            if (_mqttClient->publish( topic, payload, retain))
            {
                delete[] payload;
                delete data;
                delete cont;
                systemLog(tINFO, "Message successfully published!");
            }
            else
            {
                mqttDataMsgs->unshift( cont);
                systemLog(tERROR, "There was a problem with message publishing!");
            }
        }
        else
        {
            char logData[256];
            sprintf( logData, "Publishing message to topic: %s", topic);
            systemLog(tINFO, logData);

            /* Create JSON */
            String payloadStr;
            DynamicJsonDocument paramPublishDoc(512);
            char Time[TIME_STRING_LENGTH];

            paramPublishDoc.clear();
            paramPublishDoc["device_id"] = SystemGetDeviceId();
            paramPublishDoc["name"] = data->name;
            paramPublishDoc["time"] = getSystemTimeString( Time, (data->unix_ms)/1000);
            paramPublishDoc["unix_ms"] = data->unix_ms;

            if (data->flags & 0x02)
            {
                paramPublishDoc["value"] = data->text;
            }
            else
            {
                paramPublishDoc["value"] = data->value;
            }

            serializeJson(paramPublishDoc, payloadStr);

            /* Publish */
            if (_mqttClient->publish( topic, payloadStr.c_str(), retain))
            {
                delete[] data->name;
                if (data->flags & 0x02)
                {
                    delete[] data->text;
                }
                delete data;
                delete cont;
                systemLog(tINFO, "Message successfully published!");
            }
            else
            {
                mqttDataMsgs->unshift( cont);
                systemLog(tERROR, "There was a problem with message publishing!");
            }
        }
    }
    /* SEND BATCH DATA */
    else
    {
        char topicChar[64];
        sprintf( topicChar, "/%s/devices/%s/%s", MqttGetUser(), SystemGetDeviceId(), "params");

        int const maxPayloadLen = 1024;
        char payload[maxPayloadLen];
        char *end = payload;
        end += sprintf( payload, "{\"device_id\":\"%s\",\"data\":[", SystemGetDeviceId());

        end += BatchGetLastData( end);
        while (BatchNumOfElements() && end-payload < maxPayloadLen*8/10)
        {
            *end = ',';
            end++;
            end += BatchGetLastData( end);
        }
        *end = ']';
        end++;
        *end = '}';
        end++;
        *end = '\0';
        if (_mqttClient->publish(topicChar, payload, false))
        {
            systemLog(tINFO, "Batch Message successfully published!");
        }
        else
        {
            systemLog(tERROR, "There was a problem with batch message publishing!");
        }

        if (!BatchNumOfElements())
        {
            LastBatchPubTime = TimeNow;
        }
    }
}


/**
 * Triggers (forces) MQTT publishing from external call
 */
void triggerMqttPublish_Extern()
{
    publishMqttMessages();
}