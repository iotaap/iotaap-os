#include "mqtt_dataDevice.h"

#include "./system/definitions.h"
#include "./libs_3rd_party/ArduinoJson-v6.18.4/ArduinoJson-v6.18.4.h"
#include "./system/utils.h"
#include "./system/system_tasks.h"
#include "./fs/local_data.h"
#include "./mqtt/mqtt_client.h"
#include "./fs/sys_cfg.h"
#include "./system/system_configuration.h"


/**
 * @brief Creates and publishes parameter document to cloud or local
 * 
 * @param name Value name
 * @param value Numeric Value
 * @param time Time
 * @return int - expects 0 return on success
 */
int uDeviceCloudPublishParam(const char *name, float value)
{
    return uDeviceCloudPublish( "params", name, value);
}

/**
 * @brief Creates and publishes parameter document to cloud or local
 * 
 * @param name Value name
 * @param value Non-numeric Value 
 * @param time Time
 * @return int 
 */
int uDeviceCloudPublishParam(const char *name, const char *value)
{
    return uDeviceCloudPublish( "params", name, value);
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

int uDeviceCloudPublish(const char *uTopic, const char *uName, const char *value)
{
    /* Generate endpoint url and publish */
    char topicChar[256];
    sprintf( topicChar, "/%s/devices/%s/%s", MqttGetUser(), SystemGetDeviceId(), uTopic);
    mqttPublish(topicChar, uName, value, false);

    return 0;
}
int uDeviceCloudPublish(const char *uTopic, const char *uName, float value)
{
    /* Generate endpoint url and publish */
    char topicChar[256];
    sprintf( topicChar, "/%s/devices/%s/%s", MqttGetUser(), SystemGetDeviceId(), uTopic);
    mqttPublish(topicChar, uName, value, false);

    return 0;
}