#ifndef __IOTAAP_OS_DEFINITIONS_H__
#define __IOTAAP_OS_DEFINITIONS_H__

/* GENERAL CONFIG */
#define LIB_VERSION "4.0.0"
#define STATUS_PUBLISH_TIME 5000 // How often system status is published in ms
#define OTA_CHECK_TIME 21600000 // Period in ms for checking for OTA updates
#define CUSTOM_WDT_TIMEOUT 30 // Watchdog timeout in seconds
#define WIFI_CONNECT_TIMEOUT 5  // WiFi timeout in seconds
#undef ARDUINOJSON_USE_LONG_LONG
#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

/* QUEUE SIZES */
#define LOGS_QUEUE_SIZE 10
#define LOCAL_DATA_QUEUE_SIZE 20
#define SUBS_TOPIC_QUEUE_SIZE 10
#define UNSUBS_TOPIC_QUEUE_SIZE 10
#define MQTT_MESSAGES_QUEUE_SIZE 50

/* PINS */
#define BATSENS_PIN 36 // Battery voltage sensor pin
#define SD_CS_PIN 5    // SD Card CS pin
#define LED1 2         // Onboard LED1 pin
#define CONFIGURATOR_BUTTON     25
#define CONFIGURATOR_ENTER_MS   1000

/* FS */
#define SYSTEM_LOG_DIR                "var/log/"    // System logs directory
#define SYSTEM_FILENAME_TEMPLATE      "SYS%05d.log" // Template for system log file name
#define SYSTEM_FULL_FILEPATH_TEMPLATE "var/log/SYS%05d.log" // Template for system log file full path and name
#define MAX_SYSTEM_LOG_FILE_SIZE      50            // File max size (50MB)
#define MAX_SYSTEM_LOGS_SIZE          2048          // Max is 2048 MB (2 GB)
#define MAX_SYSTEM_LOGS_FILE_NUM      (MAX_SYSTEM_LOGS_SIZE/MAX_SYSTEM_LOG_FILE_SIZE)
/* CA certificate path */
#define CA_CRT_DIR          "/"
#define CA_CRT_PATH         CA_CRT_DIR"ca.crt"
/* System configuration file path */
#define CFG_DIR             "/"
#define SYS_CFG_PATH        CFG_DIR"default.cfg"
/* Custom configuration file path */
#define USER_CFG_PATH       CFG_DIR"custom.cfg"
/* System temporary publishing data log path */
#define LOCAL_DATA_DIR      "home/iotaap/"
#define LOCAL_DATA_PATH     LOCAL_DATA_DIR"data.log"

/* OTA */
#define OTA_CHECK_DEVICE_URL "https://ota.iotaap.io/v1/ota/device/latest/"      // OTA update check endpoint for device
#define OTA_CHECK_GROUP_URL "https://ota.iotaap.io/v1/ota/group/latest/"        // OTA update check endpoint for group
#define OTA_DOWNLOAD_DEVICE_URL "https://ota.iotaap.io/v1/ota/device/download/" // OTA update download endpoint for device
#define OTA_DOWNLOAD_GROUP_URL "https://ota.iotaap.io/v1/ota/group/download/"   // OTA update download endpoint for group

#define TIME_STRING_LENGTH  64

/* Maximum stack memory used in task */
#if (0)
    #define PRINT_EXTRA_STACK_IN_TASK()                             \
    {                                                               \
        static int minfree = 0xFFFFFF;                              \
        int everminfree = uxTaskGetStackHighWaterMark(NULL);        \
        if (everminfree < minfree)                                  \
        {                                                           \
            minfree = everminfree;                                  \
            Serial.printf("%s = %d\n", __func__, everminfree);      \
        }                                                           \
    } while(0);
#else
#define PRINT_EXTRA_STACK_IN_TASK()
#endif

#endif