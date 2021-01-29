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
    char wifiSSID[32];
    char wifiPASS[32];
    /* Restart on fail to connect to WiFi */
    bool rof;
};



/* WIFI status */
sWifiStat wifiStat;
/* WIFI configuration */
sWifiConfig wifiConfig;
/* Secure connection - EXTERN - shared between MQTT and OTA update */
WiFiClientSecure wifiClientSecure;

/**
 * Data from JSON in structure
 */
struct sJsonKeys JsonWifiData[] = 
{
    { wifiConfig.wifiSSID, JsonDataTypeString_32, "ssid"     },
    { wifiConfig.wifiPASS, JsonDataTypeString_32, "ssid_pass"},
    {&wifiConfig.rof     , JsonDataTypeBool     , "rof"      },
};


static void WiFiTask( void *parameter);

/******************************************************************************/
/***** GLOBAL FUNCTIONS *******************************************************/
/******************************************************************************/

/**
 * @brief Start WiFi process task
 */
static TaskHandle_t WiFiHandler = NULL;
void wifiConnect( void)
{
    xTaskCreate(
        WiFiTask,
        "WiFiProcess",
        10000,
        NULL,
        1,
        &WiFiHandler);
}

/**
 * @brief Save data from JSON document to local structure
 */
void InitWificonfigDataFromJsonDocument( DynamicJsonDocument ConfigJson)
{
    int StructSize = sizeof(JsonWifiData)/sizeof(*JsonWifiData);
    InitDataFromSystemJson( ConfigJson, JsonWifiData, StructSize);

#if (0)
    Serial.println();
    Serial.print("W1:");
    Serial.println(wifiConfig.wifiSSID);
    Serial.print("W2:");
    Serial.println(wifiConfig.wifiPASS);
    Serial.print("W3:");
    Serial.println(wifiConfig.rof);
    Serial.println();
#endif
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

/**
 * @brief Task will take care of maintaining WiFi connection
 * 
 * @param parameter 
 */
static void WiFiTask( void *parameter)
{
    int timeoutCounter = 0;
    LedBlinkSlow();
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiConfig.wifiSSID, wifiConfig.wifiPASS);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    wifiStat.wifiConnected = WiFi.status() == WL_CONNECTED;

    while (1)
    {
        if (!wifiStat.wifiConnected)
        {
            LedBlinkSlow();
            systemLog(tERROR, "WiFi connection broken");
            timeoutCounter++;
            if (timeoutCounter >= WIFI_CONNECT_TIMEOUT * 10)
            {
                if (wifiConfig.rof)
                {
                    ESP.restart();
                }
                else
                {
                    systemLog(tERROR, "Could not connect to the WiFI AP");
                    WiFi.mode(WIFI_STA);
                    WiFi.begin(wifiConfig.wifiSSID, wifiConfig.wifiPASS);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    timeoutCounter = 0;
                }
            }
        }
        else
        {
            LedBlinkPeriodic();
        }
        wifiStat.wifiConnected = WiFi.status() == WL_CONNECTED;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
