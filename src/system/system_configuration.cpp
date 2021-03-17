#include "system_configuration.h"

#include "./system/system_json.h"


struct sSystemConfig
{
    char deviceID[30];
    char deviceToken[30];
    char groupID[30];
    char groupToken[30];
    int timezone; // Only signed number offsetting from GMT Timezone
    char ntp1[20];   // First NTP server
    char ntp2[20];   // Second NTP server
    const char *fwVersion;
    bool automaticUpdates; // True if automatic updates are turned on
    char CAcert[8192];
};

sSystemConfig systemConfig =
{
    "",                 /* deviceID         */
    "",                 /* deviceToken      */
    "",                 /* groupID          */
    "",                 /* groupToken       */
    4,                  /* timezone         */
    "pool.ntp.org",     /* ntp1             */
    "time.nist.gov",    /* ntp2             */
    "",                 /* fwVersion        */
    false,              /* automaticUpdates */
    ""                  /* CAcert           */
};

/* Data from JSON in structure */
struct sJsonKeys JsonSysData[] = 
{
    { systemConfig.deviceID        , JsonDataTypeString_30, "device_id"   , "Device ID"   },
    { systemConfig.deviceToken     , JsonDataTypePass_30  , "device_token", "Device Token"},
    { systemConfig.groupID         , JsonDataTypeString_30, "group_id"    , "Group ID"    },
    { systemConfig.groupToken      , JsonDataTypePass_30  , "group_token" , "Group Token" },
    {&systemConfig.automaticUpdates, JsonDataTypeBool     , "auto_update" , "Auto Update" },
    { systemConfig.ntp1            , JsonDataTypeString_20, "ntp_1"       , "NTP 1"       },
    { systemConfig.ntp2            , JsonDataTypeString_20, "ntp_2"       , "NTP 2"       },
    {&systemConfig.timezone        , JsonDataTypeInt      , "timezone"    , "Time zone"   }
};



/******************************************************************************/
/***** GLOBAL FUNCTIONS *******************************************************/
/******************************************************************************/

/**
 * @brief Saves data from JSON document to local structure
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
