#include "system_configuration.h"

#include "./system/system_json.h"


struct sSystemConfig
{
    char deviceID[30];
    char deviceToken[30];
    char groupID[30];
    char groupToken[30];
    int timezone; // Only signed number form GMT
    char ntp1[20];   // First NTP server
    char ntp2[20];   // Seconf NTP server
    const char *fwVersion;
    bool automaticUpdates; // True if automatic updates are turned on
    char CAcert[8192];
};

sSystemConfig systemConfig;

/* Data from JSON in structure */
struct sJsonKeys JsonSysData[] = 
{
    { systemConfig.deviceID        , JsonDataTypeString_30, "device_id"   },
    { systemConfig.groupID         , JsonDataTypeString_30, "group_id"    },
    { systemConfig.deviceToken     , JsonDataTypeString_30, "device_token"},
    { systemConfig.groupToken      , JsonDataTypeString_30, "group_token" },
    {&systemConfig.automaticUpdates, JsonDataTypeBool     , "auto_update" },
    { systemConfig.ntp1            , JsonDataTypeString_20, "ntp_1"       },
    { systemConfig.ntp2            , JsonDataTypeString_20, "ntp_2"       },
    {&systemConfig.timezone        , JsonDataTypeInt      , "timezone"    }
};



/******************************************************************************/
/***** GLOBAL FUNCTIONS *******************************************************/
/******************************************************************************/

/**
 * @brief Save data from JSON document to local structure
 */
void InitSystemConfigDataFromJsonDocument( DynamicJsonDocument ConfigJson)
{
    int StructSize = sizeof(JsonSysData)/sizeof(*JsonSysData);
    InitDataFromSystemJson( ConfigJson, JsonSysData, StructSize);

#if (0)
    Serial.println();
    Serial.print("S1:");
    Serial.println(systemConfig.deviceID);
    Serial.print("S2:");
    Serial.println(systemConfig.groupID);
    Serial.print("S3:");
    Serial.println(systemConfig.deviceToken);
    Serial.print("S4:");
    Serial.println(systemConfig.groupToken);
    Serial.print("S5:");
    Serial.println(systemConfig.automaticUpdates);
    Serial.print("S6:");
    Serial.println(systemConfig.ntp1);
    Serial.print("S7:");
    Serial.println(systemConfig.ntp2);
    Serial.print("S8:");
    Serial.println(systemConfig.timezone);
    Serial.println();
#endif
}


/******************************************************************************/
/***** DATA FUNCTIONS *********************************************************/
/******************************************************************************/

/**
 * @brief Set version pointer
 */
void SystemSetVersion( const char *Version)
{
    systemConfig.fwVersion = Version;
}

/**
 * @brief Get version pointer
 */
const char *SystemGetFwVersion( void)
{
    return systemConfig.fwVersion;
}

/**
 * @brief Get device ID
 */
const char *SystemGetDeviceId( void)
{
    return systemConfig.deviceID;
}

/**
 * @brief Get group ID
 */
const char *SystemGetGroupId( void)
{
    return systemConfig.groupID;
}

/**
 * @brief Get device token
 */
const char *SystemGetDeviceToken( void)
{
    return systemConfig.deviceToken;
}

/**
 * @brief Get group token
 */
const char *SystemGetGroupToken( void)
{
    return systemConfig.groupToken;
}

/**
 * @brief Get NTP server 1
 */
const char *SystemGetNtp1( void)
{
    return systemConfig.ntp1;
}

/**
 * @brief Get NTP server 2
 */
const char *SystemGetNtp2( void)
{
    return systemConfig.ntp2;
}

/**
 * @brief Get system timezone
 */
int SystemGetTimezone( void)
{
    return systemConfig.timezone;
}

/**
 * @brief Get system automatic update flag
 */
bool SystemGetAutoUpdateFlag( void)
{
    return systemConfig.automaticUpdates;
}

/**
 * @brief Get CA certificate
 */
char *SystemGetCAcertificate( void)
{
    return systemConfig.CAcert;
}
