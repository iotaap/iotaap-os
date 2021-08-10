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
#include "./mqtt/mqtt_remoteControl.h"

HTTPClient updatesHttpClient;

DynamicJsonDocument updateResponse(64);
char updateResponseChar[64];

void sendUpdateResponse(int code);
bool updateCheckRequestedPrev;

/**
 * @brief Checks if updates are available, if all conditions are met then runs checking for update accordingly
 * 
 */
void handleUpdates()
{
    updateCheckRequestedPrev = systemStat.updateCheckRequested;
    if (SystemGetAutoUpdateFlag() || systemStat.updateCheckRequested)
    {
        /* Measures elapsed time between update checks */
        static uint64_t updateTimingPrev = 0;
        uint64_t updateTimingNow = millis();

        systemStat.updateInProgress = false;

        if ((WifiIsConnected() && !systemStat.updateInProgress) &&
            (((updateTimingNow - updateTimingPrev) >= OTA_CHECK_TIME) ||
             systemStat.updateCheckRequested))
        {
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
    /* Do not check for update if no OTA server */
    if (!strlen(SystemGetOtaServer()))
    {
        return;
    }
    systemLog(tSYSTEM, "Checking for updates");

    /* Notify cloud that update request is received */
    if(systemStat.updateCheckRequested){
        sendUpdateResponse(200);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Wait at least 2s to be sure that response is published to the cloud

    DynamicJsonDocument versionJson(128);
    
    wifiClient->stop();
    
    if (strlen(SystemGetGroupId()) == 0)
    { // Checking either this device is part of a group or a standalone device
        updatesHttpClient.begin(*wifiClient, String(SystemGetOtaServer()) + OTA_CHECK_DEVICE_URI + String(SystemGetDeviceId()));
    }
    else
    {
        updatesHttpClient.begin(*wifiClient, String(SystemGetOtaServer()) + OTA_CHECK_GROUP_URI + String(SystemGetGroupId()));
    }

    int httpCode = updatesHttpClient.GET(); // return code for HTTP Get request

    if (httpCode == 200)
    { // Sucess return code
        String payload = updatesHttpClient.getString();
        deserializeJson(versionJson, payload);

        String version = versionJson["ver"];

        if (version != SystemGetFwVersion())
        {
            systemLog(tSYSTEM, "New update available!");
            /* Notify cloud that update process started */
            if(systemStat.updateCheckRequested){
                sendUpdateResponse(201);
            }
            systemStat.updateCheckRequested = false;
            otaUpdate();
        }
        else
        {
            updatesHttpClient.end();
            systemLog(tSYSTEM, "No new updates");
            /* Notify cloud that versions are the same */
            if(systemStat.updateCheckRequested){
                sendUpdateResponse(202);
            }
        }
        systemStat.updateCheckRequested = false;
    }
    else if (httpCode == 404)
    { // Not found return code
        updatesHttpClient.end();
        systemLog(tSYSTEM, "No firmware available");
        /* Notify cloud that there is an error with update */
        if(systemStat.updateCheckRequested){
            sendUpdateResponse(203);
        }
    }
    else
    {   //return code not identified
        updatesHttpClient.end();
        systemLog(tERROR, "Cannot check for updates");
        /* Notify cloud that there is an error with update */
        if(systemStat.updateCheckRequested){
            sendUpdateResponse(203);
        }
    }
    systemStat.updateCheckRequested = false;
    wifiClient->stop();

}

/**
 * @brief Begin OTA update. Firmware image will be downloaded in the background and device will be restarted if it was successfull. System
 * will be then booted to the new firmware. This approach minimizes downtime.
 * 
 */
void otaUpdate()
{
    systemLog(tSYSTEM, "Starting update process...");
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
        ret = httpUpdate.update(*wifiClient,
                    String(SystemGetOtaServer()) + OTA_DOWNLOAD_DEVICE_URI +
                    String(SystemGetDeviceId()) + String(SystemGetDeviceToken()));
    }
    else
    {
        ret = httpUpdate.update(*wifiClient,
                    String(SystemGetOtaServer()) + OTA_DOWNLOAD_GROUP_URI +
                    String(SystemGetGroupId()) + String(SystemGetGroupToken()));
    }

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        char errBuffStr[128];
        sprintf(errBuffStr, "Update failed with error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        systemLog(tERROR, errBuffStr);
        /* Notify cloud that there is an error with update */
        if(updateCheckRequestedPrev){
            updateCheckRequestedPrev = false;
            sendUpdateResponse(203);
        }
        systemStat.updateInProgress = false;
        break;

    case HTTP_UPDATE_NO_UPDATES:
        systemLog(tINFO, "No new updates available");
        /* Notify cloud that there is an error with update */
        if(updateCheckRequestedPrev){
            updateCheckRequestedPrev = false;
            sendUpdateResponse(203);
        }
        systemStat.updateInProgress = false;
        break;

    case HTTP_UPDATE_OK:
        systemLog(tSYSTEM, "Update successfull!");
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

/**
 * Send update response based on code
 */
void sendUpdateResponse(int code){
    updateResponse["code"] = code;
    serializeJson(updateResponse, updateResponseChar);
    MqttRespondToUpdateRequest(updateResponseChar);
}