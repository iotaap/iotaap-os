#include "IoTaaP_OS.h"
#include "./wifi/wifi_module.h"
#include "./mqtt/mqtt_client.h"
#include "./mqtt/mqtt_dataDevice.h"
#include "./mqtt/mqtt_dataDeviceBatch.h"
#include "./mqtt/mqtt_dataUser.h"
#include "./system/system_init.h"
#include "./system/system_tasks.h"
#include "./system/system_configuration.h"
#include "./system/system_json.h"
#include "./fs/fs_module.h"
#include "./fs/user_cfg.h"
#include "./fs/sys_logs_data.h"
#include "./ota/ota_module.h"
#include "./libs_3rd_party/micro-sdcard/mySD.h"
#include "./configurator/configurator.h"
#include <HTTPClient.h>
#include <HTTPUpdate.h>


/**
 * @brief Construct a new IoTaaP_OS:: IoTaaP_OS object
 * 
 */
IoTaaP_OS::IoTaaP_OS(const char *fwVersion)
{
    SystemSetVersion( fwVersion);
}

/**
 * @brief Start IoTaaP OS
 * 
 */
void IoTaaP_OS::start(){
    systemInit();
    createSystemTasks();
}

/**
 * @brief Start WiFi connection
 * 
 */
void IoTaaP_OS::startWifi()
{
    if (!WifiCredentialsExist() || IsConfiguratorActive())
    {
        startConfigurator();
    }
    else
    {
        wifiConnect();
    }
}

/**
 * @brief Start MQTT connection and register callback
 * 
 */
void IoTaaP_OS::startMqtt(MQTT_CALLBACK_SIGNATURE)
{
    MqttSetCallback( callback);
    if (WifiCredentialsExist() && !IsConfiguratorActive())
    {
        connectToMqtt();
    }
}


/**
 * @brief Publishes payload to the device topic
 * 
 * @param payload - Payload (JSON)
 * @param uTopic - Topic to publish to
 * @return int Returns 0 if successful
 */
int IoTaaP_OS::deviceCloudPublish(const char *payload, const char *uTopic)
{
    return uDeviceCloudPublish(payload, uTopic);
}

/**
 * @brief Publishes payload to the topic. Root topic (username) will be automatically added
 * 
 * @param payload - Payload (JSON)
 * @param uTopic - Topic to publish to
 * @return int - Returns 0 if successful
 */
int IoTaaP_OS::basicCloudPublish(const char *payload, const char *uTopic)
{
    return uBasicCloudPublish(payload, uTopic);
}

/**
 * @brief Subscribe to a specific topic. Root topic (username) will be added automatically
 * 
 * @param uTopic - Topic to subscribe to
 * @return int Returns 0 if successful
 */
int IoTaaP_OS::basicSubscribe(const char *uTopic)
{
    return uBasicSubscribe(uTopic);
}

/**
 * @brief Unsubscribe from the specific topic
 * 
 * @param uTopic Topic (without root)
 * @return int Returns 0 if successfull
 */
int IoTaaP_OS::basicUnsubscribe(const char *uTopic)
{
    return uBasicUnsubscribe(uTopic);
}

/**
 * @brief Subscribe to a specific topic. Root topic (username) will NOT be added automatically
 * 
 * @param uTopic - Topic to subscribe to
 * @return int Returns 0 if successful
 */
int IoTaaP_OS::customSubscribe(const char *uTopic)
{
    return uCustomSubscribe(uTopic);
}

/**
 * @brief Unsubscribe from the specific topic. Username will not be added
 * 
 * @param uTopic Topic
 * @return int Returns 0 if successfull
 */
int IoTaaP_OS::customUnsubscribe(const char *uTopic)
{
    return uCustomUnsubscribe(uTopic);
}

/**
 * @brief Publishes measured parameter to the cloud in predefined format (Numeric input)
 * 
 * @param name Parameter name
 * @param value Numeric parameter value
 * @return int Returns 0 if successfully published
 */
int IoTaaP_OS::deviceCloudPublishParam(const char *name, float value)
{
    return uDeviceCloudPublishParam(name, value);
}

/**
 * @brief Publishes measured parameter to the cloud in predefined format (Non-Numeric input)
 * 
 * @param name Parameter name
 * @param value Non-numeric parameter value
 * @return int Returns 0 if successfully published
 */
int IoTaaP_OS::deviceCloudPublishParam(const char *name, const char *value)
{
    return uDeviceCloudPublishParam(name, value);
}

/**
 * @brief Publishes measured parameter to the cloud in predefined format (Numeric input)
 * 
 * @param name Parameter name
 * @param value Numeric parameter value
 * @return int Returns 0 if successfully published
 */
int IoTaaP_OS::deviceCloudPublishParamBatch(const char *name, float value)
{
    return uDeviceCloudPublishParamBatch(name, value);
}

/**
 * @brief Publishes measured parameter to the cloud in predefined format (Non-Numeric input)
 * 
 * @param name Parameter name
 * @param value Non-numeric parameter value
 * @return int Returns 0 if successfully published
 */
int IoTaaP_OS::deviceCloudPublishParamBatch(const char *name, const char *value)
{
    return uDeviceCloudPublishParamBatch(name, value);
}

/**
 * @brief Writes data to system logs
 * 
 * @param data string to write to system logs
 */
void IoTaaP_OS::writeToSystemLogs(const char *data)
{
    return uWriteToSystemLogs(data);
}

/* User parameter functions are not implemented since writing user parameters to internal FAT is not implemented */
// /**
//  * @brief Get parameter from user config file
//  * 
//  * @param element   JSON tag string
//  * @param output    Pointer to result
//  * 
//  * @return true - tag exist
//  *         false - tag does not exist
//  */
// bool IoTaaP_OS::getUserParameter(const char *element, char *output)
// {
//     return uGetUserParameter(element, output);
// }

// /**
//  * @brief Get parameter from user config file
//  * 
//  * @param element   JSON tag string
//  * @param output    Pointer to result
//  * 
//  * @return true - tag exist
//  *         false - tag does not exist
//  */
// bool IoTaaP_OS::getUserParameter(const char *element, int *output)
// {
//     return uGetUserParameter(element, output);
// }

// /**
//  * @brief Get parameter from user config file
//  * 
//  * @param element   JSON tag string
//  * @param output    Pointer to result
//  * 
//  * @return true - tag exist
//  *         false - tag does not exist
//  */
// bool IoTaaP_OS::getUserParameter(const char *element, bool *output)
// {
//     return uGetUserParameter(element, output);
// }

/**
 * @brief Get parameter from system config file
 * 
 * @param element   JSON tag string
 * @param output    Pointer to result
 * 
 * @return true - tag exist
 *         false - tag does not exist
 */
bool IoTaaP_OS::getSystemParameter(const char *element, char *output)
{
    return uGetSystemParameter(element, output);
}

/**
 * @brief Get parameter from system config file
 * 
 * @param element   JSON tag string
 * @param output    Pointer to result
 * 
 * @return true - tag exist
 *         false - tag does not exist
 */
bool IoTaaP_OS::getSystemParameter(const char *element, int *output)
{
    return uGetSystemParameter(element, output);
}

/**
 * @brief Get parameter from system config file
 * 
 * @param element   JSON tag string
 * @param output    Pointer to result
 * 
 * @return true - tag exist
 *         false - tag does not exist
 */
bool IoTaaP_OS::getSystemParameter(const char *element, bool *output)
{
    return uGetSystemParameter(element, output);
}

/**
 * @brief Triggers manual check for updates
 * 
 */
void IoTaaP_OS::checkForUpdates()
{
    SystemStatUpdateRequest();
}
