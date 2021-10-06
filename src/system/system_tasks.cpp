#include "system_tasks.h"
#include "./ota/ota_module.h"
#include "./system/definitions.h"
#include "./system/utils.h"
#include "./hmi/commands_serial.h"
#include "./libs_3rd_party/ArduinoJson-v6.18.4/ArduinoJson-v6.18.4.h"
#include "./fs/fs_module.h"
#include "./hmi/led_task.h"
#include "./fs/sys_logs_data.h"
#include "./wifi/wifi_module.h"
#include "./fs/sys_cfg.h"
#include "./system/system_configuration.h"
#include "./configurator/configurator.h"
#include "./mqtt/mqtt_subscription.h"
#include <time.h>
#include <sys/time.h>

sSystemStat systemStat;
uint64_t LastActiveTimeBeforeRestart __attribute__ ((section (".noinit")));
int LastActiveTimeZoneBeforeRestart __attribute__ ((section (".noinit")));

// Task handlers
static TaskHandle_t SyncTimehandler = NULL;
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
        8000,
        NULL,
        1,
        &FSmanagerHandler);

    xTaskCreate(
        SyncTimeTask,
        "SyncTimeprocess",
        1024,
        NULL,
        1,
        &SyncTimehandler);

    xTaskCreate(
        SerialTask,
        "SerialProcess",
        8000,
        NULL,
        1,
        &SerialHandler);
}


/**
 * @brief   Initialize time, if device was restarted by SW, restore time
 */
void InitTime( void)
{
    if (esp_reset_reason() == ESP_RST_SW && LastActiveTimeBeforeRestart)
    {
        systemStat.epochTimeUpdatedMs = 0;
        systemStat.epochTimeMs = LastActiveTimeBeforeRestart;
        systemStat.bootTimeEpochMs = LastActiveTimeBeforeRestart;
        SystemSetTimezone( LastActiveTimeZoneBeforeRestart);
        systemLog(tSYSTEM, "System time restored");

    }

    LastActiveTimeBeforeRestart = 0;
}

/**
 * @brief   Remember restart time (to restore time at startup)
 */
void SetRestartTime( void)
{
    LastActiveTimeBeforeRestart = getUnixTimeMs();
    LastActiveTimeZoneBeforeRestart = SystemGetTimezone();
} 

/**
 * @brief Update local system time
 */
void UpdateTime( uint64_t epoch_ms)
{
    systemStat.epochTimeUpdatedMs = millis();
    systemStat.epochTimeMs = epoch_ms;

    if (!systemStat.systemTimeSynced)
    {
        systemStat.systemTimeSynced = true;
        systemStat.bootTimeEpochMs = epoch_ms;
        systemLog(tSYSTEM, "Synced system time with server");
    }
    else
    {
        systemLog(tINFO, "Synced system time with server");
    }

    mqttUnsubscribe( MQTT_TIME_TOPIC);
}

/**
 * @brief Subscribe to time topic once in a while to fetch data. After fetching data
 *          unsubscribe in UpdateTime() function
 */
void SyncTimeTask(void *parameter)
{
    systemStat.systemTimeSynced = false;

    while (1)
    {
        PRINT_EXTRA_STACK_IN_TASK();
        mqttSubscribe( MQTT_TIME_TOPIC);
        vTaskDelay( MQTT_TIME_UPDATE_PERIOD_S*1000 / portTICK_PERIOD_MS);
    }
}


/**
 * @brief Set check for updates flag
 */
void SystemStatUpdateRequest( void)
{
    systemStat.updateCheckRequested = true;
}
