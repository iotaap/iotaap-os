#include "fs_module.h"

/**
 * This module initialize SD card and run file system. If initialization fails
 * ESP will be restarted. In task, system is filling file with data from queue 
 * and sends data to web(data) or serial interface(system logs).
 */

#include "./libs_3rd_party/micro-sdcard/mySD.h"
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
    systemLog(tSYSTEM, "Initializing filesystem");
    if (!SD.begin(SD_CS_PIN) || !CheckDirStructure())
    {
        char logBuff[255];
        char Time[TIME_STRING_LENGTH];
        systemStat.fsInitialized = false;
        /*Huge system issue, SD card not present or broken, display issue on debug port*/
        sprintf(logBuff, "[%s] [%s] - %s", getSystemTimeString(Time), "ERROR", "Filesystem initialization failed");
        Serial.println(logBuff);
        vTaskDelay(500 / portTICK_PERIOD_MS); // 500ms
        ESP.restart();
    }
    else
    {
        systemStat.fsInitialized = true;
        systemLog(tSYSTEM, "Filesystem initialized");
    }

    systemStat.sysCfgExist = SD.exists( (char *)SYS_CFG_PATH);
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
        if (systemStat.fsInitialized)
        {
            handleSystemLogs();
            handleAndPublishLocalData();
            sendSystemLogsToSerial();
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
    File CertFile = SD.open(path, FILE_READ);
    if (CertFile)
    {
        unsigned int fileSize = CertFile.size();
        while (CertFile.available())
        {
            CertFile.read(buffer, fileSize);
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
        CA_CRT_DIR,
        CFG_DIR,
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
