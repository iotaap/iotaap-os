#include "system_init.h"
#include "utils.h"
#include "definitions.h"
#include "./fs/fs_module.h"
#include "esp_task_wdt.h"
#include "./fs/user_cfg.h"
#include "./fs/sys_cfg.h"
#include "./fs/sys_logs_data.h"
#include "./mqtt/mqtt_client.h"
#include "./system/system_configuration.h"
#include "./configurator/configurator.h"

/**
 * @brief Initialize all system parameters
 * 
 */
void systemInit()
{
    esp_task_wdt_init(CUSTOM_WDT_TIMEOUT, true);
    delay(1000);

    ConfiguratorInit();
    periphInit();
    initializeFileSystem();

    InitSystemParameters();
    InitUserParameters();
    InitSystemLogs();

    loadCertificate(CA_CRT_PATH, SystemGetCAcertificate());
    systemInfo();
}

/**
 * @brief Log basic system information
 * 
 */
void systemInfo()
{
    char sysInfoBuff[512];
    sprintf(sysInfoBuff, "IoTaaP OS - v%s", SystemGetFwVersion());
    systemLog(tSYSTEM, sysInfoBuff);
    sprintf(sysInfoBuff, "Core version: %s", LIB_VERSION);
    systemLog(tSYSTEM, sysInfoBuff);
    sprintf(sysInfoBuff, "Device ID: %s", SystemGetDeviceId());
    systemLog(tSYSTEM, sysInfoBuff);
    systemLog(tSYSTEM, "I come in peace!");
}