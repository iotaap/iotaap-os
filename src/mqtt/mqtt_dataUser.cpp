#include "mqtt_dataUser.h"

#include "./mqtt/mqtt_client.h"
#include "./mqtt/mqtt_subscription.h"
#include "./system/utils.h"
#include "./system/system_configuration.h"


/**
 * @brief Publishes payloads to the MQTT topic. Root topic (username) will be automatically added
 * 
 * @param payload - Payload (JSON)
 * @param uTopic - Topic to publish to
 * @return int - Returns 0 if successful
 */
int uBasicCloudPublish(const char *payload, const char *uTopic)
{
    /* Generates endpoint url and publishes */
    char topicChar[256];
    sprintf( topicChar, "/%s/%s", MqttGetUser(), uTopic);
    mqttPublish(topicChar, payload, false);
    
    return 0;
}

/**
 * @brief Subscribe to a specific topic. Root topic (username) will be added automatically
 * 
 * @param uTopic - Topic to subscribe to
 * @return int Returns 0 if successful
 */
int uBasicSubscribe(const char *uTopic)
{
    /* Generates endpoint url and subscribes to topic */
    char topicChar[256];
    sprintf( topicChar, "/%s/%s", MqttGetUser(), uTopic);
    mqttSubscribe(topicChar);

    return 0;
}

/**
 * @brief Unsubscribe from the specific topic
 * 
 * @param uTopic Topic (without root)
 * @return int Returns 0 if successful
 */
int uBasicUnsubscribe(const char *uTopic)
{
    /* Generates endpoint url and unsubscribes */
    char topicChar[256];
    sprintf( topicChar, "/%s/%s", MqttGetUser(), uTopic);
    mqttUnsubscribe(topicChar);

    return 0;
}

/**
 * @brief Send data to storage service, without callback topic
 * 
 * @param token - IoTaaP Link Secret
 * @param name - Variable name
 * @param value - Variable value
 * @return int - Returns 0 if successful
 */
int uStorageServiceStore(const char *token, const char *name, float value){
    // Generates storage message and publishes it to storage topic
    DynamicJsonDocument doc(512);
    doc["token"] = token;
    doc["device"] = SystemGetDeviceId();
    doc["name"] = name;
    doc["value"] = value;
    doc["timestamp"] = getUnixTimeMs();

    char payloadStr[512];
    serializeJson(doc, payloadStr);
    doc.clear();

    return uCustomPublish(payloadStr, "/iotaapsys/services/storage/store");
}

/**
 * @brief Send data to storage service, with callback topic, automatically subscribes
 * 
 * @param token - IoTaaP Link Secret
 * @param name - Variable name
 * @param value - Variable value
 * @param callbackTopic - Full callback topic (including root username)
 * @return int - Returns 0 if successful
 */
int uStorageServiceStore(const char *token, const char *name, float value, const char *callbackTopic){
    uCustomSubscribe(callbackTopic);
    // Generates storage message and publishes it to storage topic
    DynamicJsonDocument doc(512);
    doc["token"] = token;
    doc["device"] = SystemGetDeviceId();
    doc["name"] = name;
    doc["value"] = value;
    doc["timestamp"] = getUnixTimeMs();
    doc["callbackTopic"] = callbackTopic;

    char payloadStr[512];
    serializeJson(doc, payloadStr);
    doc.clear();

    return uCustomPublish(payloadStr, "/iotaapsys/services/storage/store");
}

/**
 * @brief Publish to a specific topic. Root topic (username) will NOT be added automatically
 * 
 * @param payload - Payload (JSON)
 * @param uTopic - Topic to publish to
 * @return int Returns 0 if successful
 */
int uCustomPublish(const char *payload, const char *uTopic)
{
    mqttPublish(uTopic, payload, false);
    
    return 0;
}


/**
 * @brief Subscribe to a specific topic. Root topic (username) will NOT be added automatically
 * 
 * @param uTopic - Topic to subscribe to
 * @return int Returns 0 if successful
 */
int uCustomSubscribe(const char *uTopic)
{
    mqttSubscribe(uTopic);
    return 0;
}

/**
 * @brief Unsubscribe from the specific topic. Username will not be added
 * 
 * @param uTopic Topic
 * @return int Returns 0 if successfull
 */
int uCustomUnsubscribe(const char *uTopic)
{
    mqttUnsubscribe(uTopic);
    return 0;
}
