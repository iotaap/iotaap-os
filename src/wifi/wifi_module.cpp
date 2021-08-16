#include "wifi_module.h"
#include "WiFi.h"
#include "./system/definitions.h"
#include "./system/utils.h"
#include "./hmi/led_task.h"
#include "./fs/sys_logs_data.h"
#include "./system/system_json.h"


/**
 * WIFI status
 */
struct sWifiStat
{
    /* True if system is connected to WiFi */
    bool wifiConnected;
};

/* Data from JSON in structure */
struct sWifiConfig
{
    char *wifiSSID;
    char *wifiPASS;
    /* Restart on fail to connect to WiFi */
    bool rof;
};


/* WIFI status */
sWifiStat wifiStat;
/* WIFI configuration */
sWifiConfig wifiConfig =
{
    "",     /* wifiSSID */
    "",     /* wifiPASS */
    true    /* rof      */
};

/**
 * Data from JSON in structure
 */
struct sJsonKeys JsonWifiData[] = 
{
    {&wifiConfig.wifiSSID, JsonDataTypeString, "ssid"     , "SSID"              },
    {&wifiConfig.wifiPASS, JsonDataTypePass  , "ssid_pass", "SSID Password"     },
    {&wifiConfig.rof     , JsonDataTypeBool  , "rof"      , "Restart on Failure"},
};



/******************************************************************************/
/***** GLOBAL FUNCTIONS *******************************************************/
/******************************************************************************/

/**
 * @brief   Event function when WiFi connects
 */
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    wifiStat.wifiConnected = true;
    LedBlinkPeriodic();

}

/**
 * @brief   Event function when ESP got IP address
 */
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    char msg[64];
    sprintf( msg, "WiFi connected! IP address: %s", WiFi.localIP().toString().c_str());
    systemLog(tSYSTEM, msg);
}

/**
 * @brief   Event function when WiFi disconnects
 */
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    wifiStat.wifiConnected = false;

    LedBlinkSlow();
    systemLog(tERROR, "WiFi connection broken");

    if (wifiConfig.rof)
    {
        static int atemptCounter = 0;
        atemptCounter++;
        if (atemptCounter >= WIFI_CONNECT_TIMEOUT * 10)
        {
            systemLog(tERROR, "Could not connect to the WiFI AP");
            delay( 100 / portTICK_PERIOD_MS);
            ESP.restart();
        }
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiConfig.wifiSSID, wifiConfig.wifiPASS);
}


/**
 * @brief   Connect to WiFi
 */
void wifiConnect( void)
{
    WiFi.disconnect(true);

    delay(1000);

    WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

    LedBlinkSlow();

    /* Create unique hostname (will be same as SSID) */
    char hostname[32];
    createSSID(hostname);
    WiFi.setHostname(hostname);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiConfig.wifiSSID, wifiConfig.wifiPASS);
}

/**
 * @brief Save data from JSON document to local structure
 */
void InitWificonfigDataFromJsonDocument( DynamicJsonDocument ConfigJson)
{
    int StructSize = sizeof(JsonWifiData)/sizeof(*JsonWifiData);
    InitDataFromSystemJson( ConfigJson, JsonWifiData, StructSize);
}

/**
 * @brief Does wifi credentials exist
 */
bool WifiCredentialsExist( void)
{
    if (wifiConfig.wifiSSID[0])
    {
        return true;
    }

    return false;
}

/******************************************************************************/
/***** DATA FUNCTIONS *********************************************************/
/******************************************************************************/

/**
 * @brief Is wifi conenction active
 * @return true/false
 */
bool WifiIsConnected( void)
{
    return wifiStat.wifiConnected;
}

/******************************************************************************/
/***** STATIC FUNCTIONS *******************************************************/
/******************************************************************************/
