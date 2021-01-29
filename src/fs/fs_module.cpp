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


/**
 * @brief Initializes onboard SD card. It will restart ESP if failed
 * 
 */
void initializeFileSystem()
{
    systemLog(tINFO, "Initializing filesystem");
    if (!SD.begin(SD_CS_PIN))
    {
        systemStat.fsInitialized = false;
        systemLog(tERROR, "Filesystem initialization failed");
        ESP.restart();
    }
    else
    {
        systemStat.fsInitialized = true;
        systemLog(tINFO, "Filesystem initialized");
    }
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
