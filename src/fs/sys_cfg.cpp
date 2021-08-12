#include "sys_cfg.h"

/**
 * This module initialize system configuration, dynamically creates JSON document
 * and saves it to system structures.
 * User can fetch any data from JSON via uGetSystemParameter() function.
 */

#include "./system/definitions.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "FFat.h"
#include "./fs/json_memory.h"
#include "./fs/sys_logs_data.h"
#include "./mqtt/mqtt_client.h"
#include "./wifi/wifi_module.h"
#include "./system/system_configuration.h"
#include "./fs/serial_configuration.h"


static void CreateDefauleSysCfgFile( char *Path);


/**
 * @brief Load system configuration from SD card
 * @return  0 - OK
 *          1 - Failed to open file
 *          2 - Not enough memory
 */
int InitSystemParameters( void)
{
    /* Open file */
    fs::File SysCfgFile = FFat.open(SYS_CFG_PATH, FILE_READ);
    if (!SysCfgFile)
    {
        CreateDefauleSysCfgFile( (char *)SYS_CFG_PATH);
        systemLog(tERROR, "Failed to load system configuration");
        return 1;
    }

    /* Calculate memory needed for JSON */
    int MemSizeForJson = CalculateDynamicMemorySizeForJson( SysCfgFile);
    if (ESP.getMaxAllocHeap() < MemSizeForJson)
    {
        systemLog(tERROR, "System config file too big. Not enough RAM.");
        return 2;
    }

    /* Allocate memory, and deserialize JSON file */
    DynamicJsonDocument sysConfigDoc(MemSizeForJson);
    SysCfgFile.seek(0);
    deserializeJson(sysConfigDoc, SysCfgFile);

    /* Close file */
    SysCfgFile.close();

    /* Load system configuration */
    InitWificonfigDataFromJsonDocument( sysConfigDoc);
    InitSystemConfigDataFromJsonDocument( sysConfigDoc);
    InitMqttconfigDataFromJsonDocument( sysConfigDoc);

    return 0;
}


/**
 * @brief Create default system JSON
 * @details Default JSON document:
 *      {
 *          "ssid": "",
 *          "ssid_pass": "",
 *          "rof": true,
 *          "device_id": "",
 *          "device_token": "",
 *          "group_id": "",
 *          "group_token": "",
 *          "auto_update": false,
 *          "timezone": 4,
 *          "mqtt_server": "mqtt.iotaap.io",
 *          "mqtt_user": "",
 *          "mqtt_pass": "",
 *          "mqtt_port": 8883
 *      }
 */
static void CreateDefauleSysCfgFile( char *Path)
{
    DynamicJsonDocument sysConfigDoc(2);
    sysConfigDoc.clear();

    /* Send empty configuration for init only */
    InitWificonfigDataFromJsonDocument( sysConfigDoc);
    InitSystemConfigDataFromJsonDocument( sysConfigDoc);
    InitMqttconfigDataFromJsonDocument( sysConfigDoc);

    FromCfgSaveData( true);
}
