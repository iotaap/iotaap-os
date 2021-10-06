#include "system_configuration.h"

#include "./system/system_json.h"
#include "./system/system_tasks.h"


struct sSystemConfig
{
    char *deviceID;
    char *deviceToken;
    char *groupID;
    char *groupToken;
    int timezone; // Only signed number offsetting from GMT Timezone
    int timezoneOffsetMs;
    int timezoneOffsetS;
    const char *fwVersion;
    bool automaticUpdates; // True if automatic updates are turned on
    char *updatesServer;
    char *CAcert;
};

sSystemConfig systemConfig =
{
    "",                      /* deviceID         */
    "",                      /* deviceToken      */
    "",                      /* groupID          */
    "",                      /* groupToken       */
    4,                       /* timezone         */
    0,                       /* timezoneOffsetMs */
    0,                       /* timezoneOffsetS  */
    "",                      /* fwVersion        */
    false,                   /* automaticUpdates */
    "https://ota.iotaap.io", /* updatesServer    */
    NULL,                    /* CAcert           */
};

/* Data from JSON in structure */
struct sJsonKeys JsonSysData[] = 
{
    {&systemConfig.deviceID        , JsonDataTypeString, "device_id"   , "Device ID"         , NULL },
    {&systemConfig.deviceToken     , JsonDataTypePass  , "device_token", "Device Token"      , NULL },
    {&systemConfig.groupID         , JsonDataTypeString, "group_id"    , "Group ID"          , NULL },
    {&systemConfig.groupToken      , JsonDataTypePass  , "group_token" , "Group Token"       , NULL },
    {&systemConfig.automaticUpdates, JsonDataTypeBool  , "auto_update" , "Auto Update"       , "OTA"},
    {&systemConfig.updatesServer   , JsonDataTypeString, "ota_domain"  , "Auto Update Domain", "OTA"},
    {&systemConfig.timezone        , JsonDataTypeInt   , "timezone"    , "Time zone"         , NULL }
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

    systemConfig.timezoneOffsetMs = systemConfig.timezone*3600*1000;
    systemConfig.timezoneOffsetS  = systemConfig.timezone*3600;
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
 * @brief Get OTA server
 */
const char *SystemGetOtaServer( void)
{
    return systemConfig.updatesServer;
}

/**
 * @brief Get system timezone
 */
void SystemSetTimezone( int zone)
{
    systemConfig.timezone = zone;
    systemConfig.timezoneOffsetMs = systemConfig.timezone*3600*1000;
    systemConfig.timezoneOffsetS  = systemConfig.timezone*3600;
}

/**
 * @brief Get system timezone
 */
int SystemGetTimezone( void)
{
    return systemConfig.timezone;
}

/**
 * @brief Get system timezone
 */
int SystemGetTimezoneOffsetMs( void)
{
    return systemConfig.timezoneOffsetMs;
}

/**
 * @brief Get system timezone
 */
int SystemGetTimezoneOffsetS( void)
{
    return systemConfig.timezoneOffsetS;
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

/**
 * @brief Get memory for CA certificate
 */
char *SystemNewCAcertificate( int certLen)
{
    if (systemConfig.CAcert)
    {
        delete[] systemConfig.CAcert;
    }

    systemConfig.CAcert = new char[certLen];
    return SystemGetCAcertificate();
}
