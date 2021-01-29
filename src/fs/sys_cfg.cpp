#include "sys_cfg.h"

/**
 * This module initialize system configuration, dynamicaly create JSON document
 * and saves it to system structures.
 * User can fetch any data from JSON via uGetSystemParameter() function.
 */

#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "./libs_3rd_party/micro-sdcard/mySD.h"
#include "./system/definitions.h"
#include "./fs/json_memory.h"
#include "./fs/sys_logs_data.h"
#include "./mqtt/mqtt_client.h"
#include "./wifi/wifi_module.h"
#include "./system/system_configuration.h"


/**
 * @brief Load system configuration from SD card
 * @return  0 - OK
 *          1 - Failed to open file
 *          2 - Not enough memory
 */
int InitSystemParameters( void)
{
    /* Open file */
    File SysCfgFile = SD.open(SYS_CFG_PATH, FILE_READ);
    if (!SysCfgFile)
    {
        systemLog(tERROR, "Failed to load system configuration");
        return 1;
    }

    /* Calculate memory for JSON */
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
    InitSystemConfigDataFromJsonDocument( sysConfigDoc);
    InitWificonfigDataFromJsonDocument( sysConfigDoc);
    InitMqttconfigDataFromJsonDocument( sysConfigDoc);

    /* TEST CODE */
    #if 0
        char ValStr[50];
        int ValInt;
        bool ValBool;
        if (uGetSystemParameter( "mqtt_server", ValStr))    Serial.println(ValStr);
        else                                                Serial.println("Nema");
        if (uGetSystemParameter( "pivo", ValStr))           Serial.println(ValStr);
        else                                                Serial.println("Nema");
        if (uGetSystemParameter( "mqtt_port", &ValInt))     Serial.println(ValInt);
        else                                                Serial.println("Nema");
        if (uGetSystemParameter( "piva", &ValInt))          Serial.println(ValInt);
        else                                                Serial.println("Nema");
        if (uGetSystemParameter( "rof", &ValBool))          Serial.println(ValBool);
        else                                                Serial.println("Nema");
        if (uGetSystemParameter( "pive", &ValBool))         Serial.println(ValBool);
        else                                                Serial.println("Nema");
    #endif

    return 0;
}
