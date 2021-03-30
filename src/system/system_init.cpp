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
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    periphInit();
    ConfiguratorInit();
    initializeFileSystem();

    InitSystemParameters();
    // InitUserParameters(); TODO - Currently not implemented, since writing user parameters to internal FAT is not implemented
    InitSystemLogs();

    loadCertificate(CA_CRT_PATH, SystemGetCAcertificate());
    systemInfo();
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

/**
 * @brief Log basic system information
 * 
 */
void systemInfo()
{
    char sysInfoBuff[512];
    Serial.println();
    Serial.println("**************************************** IoTaaP OS ****************************************");
    Serial.println();
    sprintf(sysInfoBuff, "OS version - v%s", LIB_VERSION);
    systemLog(tSYSTEM, sysInfoBuff);
    sprintf(sysInfoBuff, "Firmware version: %s", SystemGetFwVersion());
    systemLog(tSYSTEM, sysInfoBuff);
    sprintf(sysInfoBuff, "Device ID: %s", strlen(SystemGetGroupId()) ? SystemGetGroupId() : "-");
    systemLog(tSYSTEM, sysInfoBuff);
    sprintf(sysInfoBuff, "Group ID: %s", strlen(SystemGetGroupId()) ? SystemGetGroupId() : "-");
    systemLog(tSYSTEM, sysInfoBuff);
    systemLog(tSYSTEM, "I come in peace!");
    Serial.println();
    Serial.println("***********************************************************************************************");
    Serial.println();
}