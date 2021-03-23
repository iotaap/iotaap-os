#include "mqtt_client.h"

#include "./system/queue.h"
#include "./system/definitions.h"
#include "./system/system_configuration.h"
#include "./fs/sys_cfg.h"
#include "./mqtt/mqtt_remoteControl.h"
#include "./mqtt/mqtt_subscription.h"
#include "./mqtt/mqtt_dataStatus.h"
#include "./fs/sys_logs_data.h"
#include "./hmi/led_task.h"
#include "./ota/ota_module.h"
#include "./wifi/wifi_module.h"
#include "./system/system_configuration.h"
#include "./system/system_json.h"



/**
 * MQTT status
 */
struct sMqttStat
{
    bool mqttConnected;
};

/**
 * Structure for MQTT message
 */
struct sMqttMessage
{
    String topic;
    String payload;
    bool retain;
};

/**
 * Structure for MQTT configuration
 */
struct sMqttConfig
{
    char mqttServer[20];
    char mqttUser[20];
    char mqttPassword[20];
    uint16_t port;
    MQTT_CALLBACK_SIGNATURE;
    std::function<void(char*, uint8_t*, unsigned int)> user_callback;
};



/* MQTT status */
struct sMqttStat mqttStat;
/* MQTT configuration */
struct sMqttConfig mqttConfig
{
    "mqtt1.iotaap.io",  /* mqttServer              */
    "",                 /* mqttUser                */
    "",                 /* mqttPassword            */
    8883,               /* port                    */
    NULL,               /* MQTT_CALLBACK_SIGNATURE */
    NULL                /* user_callback           */
};

/* Data from JSON in structure */
struct sJsonKeys JsonMqttData[] = 
{
    { mqttConfig.mqttServer  , JsonDataTypeString_20, "mqtt_server", "MQTT Server"  },
    { mqttConfig.mqttUser    , JsonDataTypeString_20, "mqtt_user"  , "MQTT User"    },
    { mqttConfig.mqttPassword, JsonDataTypePass_20  , "mqtt_pass"  , "MQTT Password"},
    {&mqttConfig.port        , JsonDataTypeInt      , "mqtt_port"  , "MQTT Port"    }
};
/* Queue for MQTT messaging */
Queue<sMqttMessage> mqttMessageQueue(MQTT_MESSAGES_QUEUE_SIZE);
/* MQTT uses secure network (shared with OTA) */
PubSubClient _mqttClient(wifiClientSecure);



/* Task */
static void MqttTask( void *parameter);
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
    xTaskCreate(
        MqttTask,
        "MqttProcess",
        10000,
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

#if (0)
    Serial.println();
    Serial.print("M1:");
    Serial.println(mqttConfig.mqttServer);
    Serial.print("M2:");
    Serial.println(mqttConfig.mqttUser);
    Serial.print("M3:");
    Serial.println(mqttConfig.mqttPassword);
    Serial.print("M4:");
    Serial.println(mqttConfig.port);
    Serial.println();
#endif
}


/**
 * @brief Adds new message to queue, it will be picked up by the MqttProcess task and published to topic
 * 
 * @param topic MQTT topic
 * @param payload MQTT topic payload
 * @param retain Retain last message
 */
void mqttPublish(const char *topic, const char *payload, boolean retain)
{
    // If Queue is full, data will be dropped
    mqttMessageQueue.push({topic, payload, retain});
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
    mqttSubscribeToRemoteControlTopics(); 

    mqttStat.mqttConnected = false; 
    while (1)
    {
        if (WifiIsConnected())
        {
            //Initalises security certificate
            wifiClientSecure.setTimeout(12); // timeout argument is defined in seconds for setTimeout
            wifiClientSecure.setCACert(SystemGetCAcertificate());
            _mqttClient.setBufferSize(1024);
            _mqttClient.setServer(mqttConfig.mqttServer, mqttConfig.port);
            _mqttClient.setCallback(mqttConfig.callback);
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
    }
    _mqttClient.disconnect();
    mqttStat.mqttConnected = false;
    while (1)
    {
        while (WifiIsConnected())
        {
            mqttStat.mqttConnected = _mqttClient.connected();
            if (!mqttStat.mqttConnected)
            {
                systemLog(tWARNING, "Not connected to MQTT broker");
                vTaskDelay(1000 / portTICK_PERIOD_MS); // Re-attempts to connect every second
                _mqttClient.connect(SystemGetDeviceId(), mqttConfig.mqttUser, mqttConfig.mqttPassword);
                LedBlinkFast(); // Sets LED operating into fast mode - indicates not connected
                systemLog(tINFO, "Restoring subscribed topics");
                restoreSubscribedTopics();
            }
            else
            {
                _mqttClient.loop();
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
        _mqttClient.disconnect();
        mqttStat.mqttConnected = false;     // If wifi is not connected then mqtt is not connected also
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
    }
}


/**
 * @brief Publishes MQTT messages from the queue
 */
static void publishMqttMessages( void)
{
    if (mqttMessageQueue.count() > 0)
    {
        char logData[256];
        sprintf( logData, "Publishing message to topic: %s", mqttMessageQueue.peek().topic.c_str());
        systemLog(tINFO, logData);

        if (_mqttClient.publish(mqttMessageQueue.peek().topic.c_str(),
                                mqttMessageQueue.peek().payload.c_str(),
                                mqttMessageQueue.peek().retain))
        {
            mqttMessageQueue.pop(); // Removes message being published from Queue
            systemLog(tINFO, "Message successfully published!");
        }
        else
        {
            systemLog(tERROR, "There was a problem with message publishing!");
        }
    }
}

/**
 * Triggers (forces) MQTT publishing from external call
 */
void triggerMqttPublish_Extern(){
    publishMqttMessages();
}