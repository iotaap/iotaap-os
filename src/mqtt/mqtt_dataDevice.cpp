#include "mqtt_dataDevice.h"


#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "./system/utils.h"
#include "./system/system_tasks.h"
#include "./system/definitions.h"
#include "./fs/local_data.h"
#include "./mqtt/mqtt_client.h"
#include "./fs/sys_cfg.h"
#include "./system/system_configuration.h"


DynamicJsonDocument paramPublishDoc(512);
char paramPublishPayload[512];
/**
 * @brief Creates and publishes parameter document
 * 
 * @param name Value name
 * @param value Numeric Value 
 * @param time Time
 * @return int 
 */
int uDeviceCloudPublishParam(const char *name, float value)
{
    char Time[TIME_STRING_LENGTH];

    paramPublishDoc.clear();
    paramPublishDoc["name"] = name;
    paramPublishDoc["value"] = value;
    paramPublishDoc["time"] = getSystemTimeString( Time);
    paramPublishDoc["unix_ms"] = systemStat.systemTimeMs;

    serializeJson(paramPublishDoc, paramPublishPayload);

    if (!MqttIsConnected() && systemStat.systemTimeSynced)
    {
        saveDataLocally(paramPublishPayload);
    }
    else
    {
        uDeviceCloudPublish( paramPublishPayload, "params");
    }
    return 0;
}

/**
 * @brief Creates and publishes parameter document
 * 
 * @param name Value name
 * @param value Non-numeric Value 
 * @param time Time
 * @return int 
 */
int uDeviceCloudPublishParam(const char *name, const char *value)
{
    char Time[TIME_STRING_LENGTH];

    paramPublishDoc.clear();
    paramPublishDoc["name"] = name;
    paramPublishDoc["value"] = value;
    paramPublishDoc["time"] = getSystemTimeString( Time);
    paramPublishDoc["unix_ms"] = systemStat.systemTimeMs;

    serializeJson(paramPublishDoc, paramPublishPayload);

    if (!MqttIsConnected() && systemStat.systemTimeSynced)
    {
        saveDataLocally(paramPublishPayload);
    }
    else
    {
        uDeviceCloudPublish( paramPublishPayload, "params");
    }
    return 0;
}

/**
 * @brief Publishes payload to the device topic
 * 
 * @param payload - Payload (JSON)
 * @param uTopic - Topic to publish to
 * @return int Returns 0 if successfull
 */
int uDeviceCloudPublish(const char *payload, const char *uTopic)
{
    /* Generate endpoint url and publish */
    char topicChar[256];
    sprintf( topicChar, "/%s/devices/%s/%s", MqttGetUser(), SystemGetDeviceId(), uTopic);
    mqttPublish(topicChar, payload, false);

    return 0;
}