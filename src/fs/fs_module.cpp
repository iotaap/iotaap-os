#include "fs_module.h"

/** 
 * Initializes onboard SD card and Internal FAT. It will restart ESP if FAT initialization failed.
 * This module also handles local data and logs (if SD card is present) and configuration stored
 * to internal FS.
 */

#include "./libs_3rd_party/micro-sdcard/mySD.h"
#include "FFat.h"
#include "./system/definitions.h"
#include "./fs/sys_logs_data.h"
#include "./fs/local_data.h"
#include "./fs/serial_configuration.h"
#include "./system/system_tasks.h"
#include "./system/utils.h"

static bool CheckDirStructure( void);

/**
 * @brief Initializes onboard SD card and Internal FAT. It will restart ESP if FAT initialization failed
 * 
 */
void initializeFileSystem()
{
    /* Initialize SD cart */
    Serial.println();
    systemLog(tSYSTEM, "Initializing SD filesystem");
    if (!SD.begin(SD_CS_PIN) || !CheckDirStructure())
    {
        char logBuff[255];
        char Time[TIME_STRING_LENGTH];
        systemStat.fsInitialized = false;
        /*SD card not present or broken, display warning on debug port*/
        sprintf(logBuff, "[%s] [%s] - %s", getSystemTimeString(Time), "WARNING", "SD filesystem initialization failed, data backup and logs disabled");
        Serial.println(logBuff);
        vTaskDelay(500 / portTICK_PERIOD_MS); // 500ms
    }
    else
    {
        systemStat.fsInitialized = true;
        systemLog(tSYSTEM, "SD filesystem initialized");
    }

    /* Initialize intern FS */
    systemLog(tSYSTEM, "Initializing Internal filesystem");
    if (!FFat.begin(true))
    {
        char logBuff[255];
        char Time[TIME_STRING_LENGTH];
        systemStat.fatInitialized = false;
        sprintf(logBuff, "[%s] [%s] - %s", getSystemTimeString(Time), "ERROR", "Internal filesystem initialization failed");
        Serial.println(logBuff);
        vTaskDelay(500 / portTICK_PERIOD_MS); // 500ms
        ESP.restart();
    }
    else
    {
        systemStat.fatInitialized = true;
        systemLog(tSYSTEM, "Internal filesystem initialized");
    }

    systemStat.sysCfgExist = FFat.exists( SYS_CFG_PATH);
}


/**
 * @brief Task that takes care of the filesystem
 * 
 * @param parameter 
 */
void FSmanagerTask(void *parameter)
{
    while (1)
    {
        PRINT_EXTRA_STACK_IN_TASK();
        if (systemStat.fsInitialized)
        {
            handleSystemLogs();
            handleAndPublishLocalData();
            sendSystemLogsToSerial();
        }
        if (systemStat.fatInitialized)
        {
            HandleJsonCfgFile();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
    }
}


/**
 * @brief Loads certificate from SD card
 * 
 * @param path Absolute path to the certificate
 * @param buffer Char array destination
 */
void loadCertificate(const char *path, char *buffer)
{
    fs::File CertFile = FFat.open(path, FILE_READ);
    if (CertFile)
    {
        size_t fileSize = CertFile.size();
        while (CertFile.available())
        {
            CertFile.readBytes(buffer, fileSize);
        }
        *(buffer + fileSize) = '\0'; // Terminate file
        CertFile.close();
    }
    else
    {
        systemLog(tERROR, "Failed to load SSL certificate");
    }
}

/**
 * @brief   Check if directory structure exists
 * @details If not - create new dir tree
 * 
 * @return  true/false
 */
static bool CheckDirStructure( void)
{
    const char *DirNames[] = 
    {
        SYSTEM_LOG_DIR,
        LOCAL_DATA_DIR,
    };
    int Len = sizeof(DirNames)/sizeof(*DirNames);

    for (int i=0; i<Len; i++)
    {
        if (!SD.exists( (char *)DirNames[i]))
        {
            if (!SD.mkdir( (char *)DirNames[i]))
            {
                return false;
            }
        }
    }

    return true;
}
