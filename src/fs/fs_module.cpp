#include "fs_module.h"

/**
 * This module initialize SD card and run file system. If initialization fails
 * ESP will be restarted. In task, system is filling file with data from queue 
 * and sends data to web(data) or serial interface(system logs).
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
 * @brief Initializes onboard SD card. It will restart ESP if failed
 * 
 */
void initializeFileSystem()
{
    /* Initialize SD cart */
    systemLog(tSYSTEM, "Initializing SD filesystem");
    if (!SD.begin(SD_CS_PIN) || !CheckDirStructure())
    {
        char logBuff[255];
        char Time[TIME_STRING_LENGTH];
        systemStat.fsInitialized = false;
        /*Huge system issue, SD card not present or broken, display issue on debug port*/
        sprintf(logBuff, "[%s] [%s] - %s", getSystemTimeString(Time), "ERROR", "SD Filesystem initialization failed");
        Serial.println(logBuff);
        vTaskDelay(500 / portTICK_PERIOD_MS); // 500ms
        ESP.restart();
    }
    else
    {
        systemStat.fsInitialized = true;
        systemLog(tSYSTEM, "SD Filesystem initialized");
    }

    /* Initialize intern FS */
    systemLog(tSYSTEM, "Initializing Intern filesystem");
    if (!FFat.begin(true))
    {
        char logBuff[255];
        char Time[TIME_STRING_LENGTH];
        systemStat.fatInitialized = false;
        sprintf(logBuff, "[%s] [%s] - %s", getSystemTimeString(Time), "ERROR", "Intern Filesystem initialization failed");
        Serial.println(logBuff);
        vTaskDelay(500 / portTICK_PERIOD_MS); // 500ms
        ESP.restart();
    }
    else
    {
        systemStat.fatInitialized = true;
        systemLog(tSYSTEM, "Intern Filesystem initialized");
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
