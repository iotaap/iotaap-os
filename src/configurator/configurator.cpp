#include "configurator.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "./libs_3rd_party/AsyncTCP-1.1.1/src/AsyncTCP.h"
#include "./libs_3rd_party/ESPAsyncWebServer-1.2.3/src/ESPAsyncWebServer.h"
#include "configPage.h"
#include "mbedtls/md5.h"
#include "./system/system_json.h"
#include "./system/definitions.h"
#include "./system/system_configuration.h"
#include "./fs/serial_configuration.h"
#include "./hmi/led_task.h"
#include "./system/utils.h"

#define MAX_REGISTERED_CONFS    5
AsyncWebServer server(80);
struct configParameters conf[MAX_REGISTERED_CONFS] = {0};

/* This variable will survive restart */
int ConfiguratorActive __attribute__ ((section (".noinit")));

static void createPSK( char *pass);
static void configure( AsyncWebServerRequest *request);
static void submit( AsyncWebServerRequest *request);
static void notFound( AsyncWebServerRequest *request);


/**
 * @brief   Initialize configurator
 * @details If reset reason was not SW reset, then surelly user dont want configurator
 */
void ConfiguratorInit( void)
{
    if (esp_reset_reason() != ESP_RST_SW)
    {
        ConfiguratorActive = 0;
    }

    pinMode(CONFIGURATOR_BUTTON, INPUT);
}

/**
 * @brief   Open AP with web configurator
 * @details ssid is generated with MAC address
 *          psk is generated with md5 algorithm
 *          IP is 192.168.1.8
 */
void startConfigurator( void)
{
    /* Create a Unique AP from MAC address */
    char ssid[32];
    char password[32];

    LedBlinkConfigurator();

    Serial.println();
    Serial.println("********************************** IoTaaP Web Configurator **********************************");
    Serial.println();

    IPAddress IP = IPAddress(192, 168, 1, 8);
    IPAddress gateway = IPAddress(192, 168, 1, 1);
    IPAddress NMask = IPAddress(255, 255, 255, 0);

    /* Create unique SSID ans PASS */
    createSSID( ssid);         // 
    Serial.print("SSID: ");
    Serial.println(ssid);

    createPSK( password);
    Serial.print("Password: ");
    Serial.println(password);

    /* Start AP */
    if (WiFi.mode(WIFI_AP) && WiFi.softAP(ssid, password))
    {
        WiFi.softAPConfig(IP, gateway, NMask);
    }

    Serial.println("Access Point Started");

    Serial.println();
    Serial.println("****************************************************************************************************");
    Serial.println();

    server.on("/", HTTP_GET, configure);
    server.on("/submit-config", HTTP_POST, submit);
    server.onNotFound(notFound);
    server.begin();
}

/**
 * @brief Register JSON configurations locally
 */
void configuratorRegister( struct sJsonKeys *JsonSystemData, int JsonStructSize)
{
    int id = 0;
    
    while (conf[id].Size && ++id && id<MAX_REGISTERED_CONFS);

    if (id >= MAX_REGISTERED_CONFS)
    {
        return;
    }

    conf[id].Data = JsonSystemData;
    conf[id].Size = JsonStructSize;
}

/**
 * @brief If configurator button is pressed, reset and go into configurator
 */
void HandleConfiguratorActivity( void *par)
{
    static unsigned long ButtonPressTime = 0;

    while(1)
    {
        PRINT_EXTRA_STACK_IN_TASK();
        /* Negative logic */
        int PinVal = !digitalRead(CONFIGURATOR_BUTTON);
        if (PinVal)
        {
            unsigned long TimeNow = millis();
            ButtonPressTime = ButtonPressTime ? ButtonPressTime : TimeNow;
            if (TimeNow - ButtonPressTime >= CONFIGURATOR_ENTER_MS)
            {
                ConfiguratorActivate();

                vTaskDelay( 500 / portTICK_PERIOD_MS );
                ESP.restart();
            }
        }
        else
        {
            ButtonPressTime = 0;
        }
    }
}

/**
 * @brief Activate configurator
 */
void ConfiguratorActivate( void)
{
    ConfiguratorActive = 1;
}

/**
 * @brief Deactivate configurator
 */
void ConfiguratorDeactivate( void)
{
    ConfiguratorActive = 0;
}

/**
 * @brief Check if configurator should be activated
 */
bool IsConfiguratorActive( void)
{
    if (ConfiguratorActive)
    {
        return true;
    }

    return false;
}


/**
 * @brief Fetch JSON configurations
 */
struct configParameters *configuratorGetConf( unsigned int id)
{
    return conf[id].Size ? &conf[id] : NULL;
}

/**
 * @brief Create password for wifi using md5
 */
static void createPSK( char *pass)
{
    char ssid[32];
    createSSID( ssid);

    mbedtls_md5_context _ctx;
    uint8_t outpass[16] = {0};
    mbedtls_md5_init(&_ctx);
    mbedtls_md5_starts(&_ctx);
    mbedtls_md5_update(&_ctx, (uint8_t *)ssid, strlen(ssid));
    mbedtls_md5_finish(&_ctx, outpass);
    snprintf(pass, 32, "%02X%02X%02X%02X", outpass[0], outpass[1], outpass[2], outpass[3]);
}


/**
 * @brief Serve configurator page
 */
static void configure(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/html");

    /* Start html (style + body start) */
    response->print( (String)configPageHtmlStart);

    /* Print JSON data to html elements */
    for (int i=0; conf[i].Size; i++)
    {
        struct sJsonKeys *JsDt = conf[i].Data;
        for (int j=0; j<conf[i].Size; j++, JsDt++)
        {
            switch (JsDt->ElementDataType)
            {
                case JsonDataTypeInt:
                {
                    response->print(
                            configDivInputText( JsDt->ElementDesc,
                                                JsDt->ElementKey,
                                                *(int *)JsDt->ElementPointer));
                }
                break;

                case JsonDataTypeString_20:
                case JsonDataTypeString_30:
                case JsonDataTypeString_32:
                {
                    response->print(
                            configDivInputText( JsDt->ElementDesc,
                                                JsDt->ElementKey,
                                                (char *)JsDt->ElementPointer));
                }
                break;

                case JsonDataTypePass_20:
                case JsonDataTypePass_30:
                case JsonDataTypePass_32:
                {
                    response->print(
                            configDivInputPass( JsDt->ElementDesc,
                                                JsDt->ElementKey,
                                                (char *)JsDt->ElementPointer));
                }
                break;
                
                case JsonDataTypeBool:
                {
                    response->print(
                            configDivInputRadioBool( JsDt->ElementDesc,
                                                     JsDt->ElementKey,
                                                     *(bool *)JsDt->ElementPointer));
                }
                break;
            }
        }
    }

    /* Print certificate */
    response->print( configDivInputTextArea("Certificate", "cert", (char *)SystemGetCAcertificate()));

    /* End of html */
    response->print( (String)configPageHtmlEnd);

    request->send(response);
}

/**
 * @brief Serve submit page (success/error)
 */
static void submit(AsyncWebServerRequest *request)
{
    if (!request->hasParam("cert", true))
    {
        request->send(400, "text/html", (String)errorPageHtml);
        return;
    }
    
    for (int i=0; conf[i].Size; i++)
    {
        struct sJsonKeys *JsDt = conf[i].Data;
        for (int j=0; j<conf[i].Size; j++, JsDt++)
        {
            if (!request->hasParam(JsDt->ElementKey, true))
            {
                request->send(400, "text/html", (String)errorPageHtml);
                return;
            }

            switch (JsDt->ElementDataType)
            {
                case JsonDataTypeInt:
                {
                    *(int *)JsDt->ElementPointer =
                            atoi(request->arg( JsDt->ElementKey).c_str());
                }
                break;

                case JsonDataTypeString_20:
                case JsonDataTypeString_30:
                case JsonDataTypeString_32:
                case JsonDataTypePass_20:
                case JsonDataTypePass_30:
                case JsonDataTypePass_32:
                {
                    strcpy( (char *)JsDt->ElementPointer,
                            request->arg( JsDt->ElementKey).c_str());
                }
                break;
                
                case JsonDataTypeBool:
                {
                    *(bool *)JsDt->ElementPointer =
                            (request->arg( JsDt->ElementKey) == "true") ? true : false;
                }
                break;
            }
        }
    }
    
    /* Save cert to structure */
    strcpy( SystemGetCAcertificate(), request->arg( "cert").c_str());
    /* Save data to JSON and reset */
    FromCfgSaveData( true);

    request->send(200, "text/html", (String)successPageHtml);
}


/**
 * @brief Serve error 404 not found web page
 */
static void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/html", (String)notFoundPageHtml);
}

