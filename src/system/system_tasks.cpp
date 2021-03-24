#include "system_tasks.h"
#include "./ota/ota_module.h"
#include "./system/definitions.h"
#include "./system/utils.h"
#include "./hmi/commands_serial.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "./fs/fs_module.h"
#include "./hmi/led_task.h"
#include "./fs/sys_logs_data.h"
#include "./wifi/wifi_module.h"
#include "./fs/sys_cfg.h"
#include "./system/system_configuration.h"
#include "./configurator/configurator.h"

sSystemStat systemStat;

// Task handlers
static TaskHandle_t SystemStatusHandler = NULL;
static TaskHandle_t SyncNTPhandler = NULL;
static TaskHandle_t FSmanagerHandler = NULL;
static TaskHandle_t LedTaskHandler = NULL;
static TaskHandle_t SerialHandler = NULL;
static TaskHandle_t ConfiguratorTaskHandler = NULL;

// System tasks
void createSystemTasks()
{
    xTaskCreate(
        LedTask,
        "LedProcess",
        1000,
        NULL,
        1,
        &LedTaskHandler);
        
    xTaskCreate(
        HandleConfiguratorActivity,
        "HandleConfiguratorActivity",
        2000,
        NULL,
        1,
        &ConfiguratorTaskHandler);

    xTaskCreate(
        FSmanagerTask,
        "FSmanagerProcess",
        25000,
        NULL,
        1,
        &FSmanagerHandler);

    xTaskCreate(
        SystemStatusTask,
        "SystemStatusProcess",
        1000,
        NULL,
        1,
        &SystemStatusHandler);

    xTaskCreate(
        SyncNTPtask,
        "SyncNTPprocess",
        8000,
        NULL,
        1,
        &SyncNTPhandler);

    xTaskCreate(
        SerialTask,
        "SerialProcess",
        15000,
        NULL,
        1,
        &SerialHandler);
}

/**
 * @brief Basic system task for altering system status values
 * 
 * @param parameter 
 */
void SystemStatusTask(void *parameter)
{
    while (1)
    {
        systemStat.uptime = (getSystemTimeMs() - systemStat.bootTime) / 1000;
        systemStat.batteryPerc = getBatteryPercentage();

        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
    }
}

/**
 * @brief Takes care of sync with NTP server
 * 
 * @param parameter 
 */
void SyncNTPtask(void *parameter)
{
    unsigned long int backupTime;
    systemStat.systemTimeSynced = false;
    bool previouslySynced = false;
    while (1)
    {
        if (!systemStat.systemTimeSynced && WifiIsConnected())
        {
            configTime(SystemGetTimezone() * 3600, 0, SystemGetNtp1(), SystemGetNtp2()); // Configure system time
            systemStat.systemTimeSynced = true;
            previouslySynced = true;
            getLocalTime(&systemStat.systemTime);
            systemStat.bootTime = getSystemTimeMs();
            systemLog(tSYSTEM, "Synced system time with NTP");
        }
        if (!WifiIsConnected() && systemStat.systemTimeSynced)
        {
            systemStat.systemTimeSynced = false;
        }

        if (!getLocalTime(&systemStat.systemTime) && !previouslySynced)
        {
            systemLog(tERROR, "NTP sync failed, timestamp switched from timestamp to time from boot");
            backupTime = millis();
            systemStat.systemTime.tm_hour = backupTime / 3600000;
            systemStat.systemTime.tm_min = backupTime / 60000;
            systemStat.systemTime.tm_sec = backupTime / 1000;

            systemStat.systemTime.tm_isdst = 0;
            systemStat.systemTime.tm_mday = 0;
            systemStat.systemTime.tm_mon = 0;
            systemStat.systemTime.tm_wday = 0;
            systemStat.systemTime.tm_yday = 0;
            systemStat.systemTime.tm_year = 0;
            systemStat.systemTimeMs = (int64_t)millis();
        }
        else
        {
            systemStat.systemTimeMs = getSystemTimeMs();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
    }
}


/**
 * @brief Reset system uptime to 0
 */
void SystemStatUptimeReset( void)
{
    systemStat.uptime = 0;
}

/**
 * @brief Set check for updates flag
 */
void SystemStatUpdateRequest( void)
{
    systemStat.updateCheckRequested = true;
}
