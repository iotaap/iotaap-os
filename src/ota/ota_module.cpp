#include "ota_module.h"
#include "./system/definitions.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include <HTTPUpdate.h>
#include "./system/utils.h"
#include "esp_task_wdt.h"
#include "soc/rtc_wdt.h"
#include "./fs/sys_logs_data.h"
#include "./wifi/wifi_module.h"
#include "./mqtt/mqtt_client.h"
#include "./fs/sys_cfg.h"
#include "./system/system_tasks.h"
#include "./system/system_configuration.h"

HTTPClient updatesHttpClient;

/**
 * @brief Checks if updates are available, if all conditions are met then runs checking for update accordingly
 * 
 */
void handleUpdates()
{
    if (SystemGetAutoUpdateFlag() || systemStat.updateCheckRequested)
    {
        /* Measures elapsed time between update checks */
        static uint64_t updateTimingPrev = 0;
        uint64_t updateTimingNow = millis();

        systemStat.updateInProgress = false;

        if ((systemStat.fsInitialized && WifiIsConnected() && !systemStat.updateInProgress) &&
            (((updateTimingNow - updateTimingPrev) >= OTA_CHECK_TIME) ||
             systemStat.updateCheckRequested))
        {
            systemStat.updateCheckRequested = false;
            updateTimingPrev = updateTimingNow;
            checkUpdate();
        }
    }
}

/**
 * @brief Check if new firmware version is available
 * 
 */
void checkUpdate()
{
    systemLog(tINFO, "Checking for updates");
    DynamicJsonDocument versionJson(128);
    
    wifiClientSecure.stop();
    
    if (strlen(SystemGetGroupId()) == 0)
    { // Checking either this device is part of a group or a standalone device
        updatesHttpClient.begin(wifiClientSecure, OTA_CHECK_DEVICE_URL + String(SystemGetDeviceId()));
    }
    else
    {
        updatesHttpClient.begin(wifiClientSecure, OTA_CHECK_GROUP_URL + String(SystemGetGroupId()));
    }

    int httpCode = updatesHttpClient.GET(); // return code for HTTP Get request

    if (httpCode == 200)
    { // Sucess return code
        String payload = updatesHttpClient.getString();
        deserializeJson(versionJson, payload);

        String version = versionJson["ver"];

        if (version != SystemGetFwVersion())
        {
            systemLog(tINFO, "New update available!");
            otaUpdate();
        }
        else
        {
            updatesHttpClient.end();
            systemLog(tINFO, "No new updates");
        }
    }
    else if (httpCode == 404)
    { // Not found return code
        updatesHttpClient.end();
        systemLog(tINFO, "No firmware available");
    }
    else
    {   //return code not identified
        updatesHttpClient.end();
        systemLog(tERROR, "Cannot check for updates");
    }

    wifiClientSecure.stop();

}

/**
 * @brief Begin OTA update. Firmware image will be downloaded in the background and device will be restarted if it was successfull. System
 * will be then booted to the new firmware. This approach minimizes downtime.
 * 
 */
void otaUpdate()
{
    systemLog(tINFO, "Starting update process...");
    systemStat.updateInProgress = true;
    t_httpUpdate_return ret;
    httpUpdate.setLedPin(LED1);
    httpUpdate.rebootOnUpdate(true);

    // Disable WDT before starting update - prevents WDT causing hardware reboot during update
    rtc_wdt_protect_off();
    rtc_wdt_disable();
    disableCore0WDT();
    // WDT for core 1 is disabled by default
    
    // Update respective to device group or standalone
    if (strlen(SystemGetGroupId()) == 0)
    {
        ret = httpUpdate.update(wifiClientSecure, OTA_DOWNLOAD_DEVICE_URL +
                    String(SystemGetDeviceId()) + String(SystemGetDeviceToken()));
    }
    else
    {
        ret = httpUpdate.update(wifiClientSecure, OTA_DOWNLOAD_GROUP_URL +
                    String(SystemGetGroupId()) + String(SystemGetGroupToken()));
    }

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        char errBuffStr[128];
        sprintf(errBuffStr, "Update failed with error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        systemLog(tERROR, errBuffStr);
        systemStat.updateInProgress = false;
        break;

    case HTTP_UPDATE_NO_UPDATES:
        systemLog(tINFO, "No new updates available");
        systemStat.updateInProgress = false;
        break;

    case HTTP_UPDATE_OK:
        systemLog(tERROR, "Update successfull!");
        // We do not have to restart update task since system will be restarted on successful update
        break;
    default:

        break;
    }

    // Enable WDT after update
    rtc_wdt_protect_on();
    rtc_wdt_enable();
    enableCore0WDT();
    // WDT for core 1 is disabled by default
    //
}