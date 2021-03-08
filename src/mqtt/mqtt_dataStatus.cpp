#include "mqtt_dataStatus.h"

#include "./system/definitions.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "./mqtt/mqtt_client.h"
#include "./fs/sys_cfg.h"
#include "./system/system_tasks.h"
#include "./system/system_configuration.h"


DynamicJsonDocument doc(256);
/**
 * @brief Publishing system status, everything must be done
 * in one task to avoid using mutex and semaphore
 * 
 */
void publishSystemStatus()
{
    /* Measures elapsed time between publishing device status */
    static uint64_t statusTimingPrev = 0;
    uint64_t statusTimingNow = millis();

    if ((statusTimingNow - statusTimingPrev) >= STATUS_PUBLISH_TIME)
    {
        doc["battery"] = systemStat.batteryPerc;
        doc["uptime"] = systemStat.uptime;
        doc["core_version"] = LIB_VERSION;
        doc["fw_version"] = SystemGetFwVersion();

        char deviceStatus[256];
        serializeJson(doc, deviceStatus);
        doc.clear();

        char topicChar[256];
        sprintf( topicChar, "/%s/devices/%s/status", MqttGetUser(), SystemGetDeviceId());
        mqttPublish(topicChar, deviceStatus, false);
        statusTimingPrev = statusTimingNow;
    }
}