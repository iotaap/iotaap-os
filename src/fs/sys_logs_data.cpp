#include "sys_logs_data.h"

/**
 * This module writes system logs to queue (uWriteToSystemLogs() or systemLog()),
 * and then function handleSystemLogs() transfers system logs to file.
 * 
 * Maximum file size is defined by MAX_SYSTEM_LOG_FILE_SIZE.
 * Maximum number of files is defined by MAX_SYSTEM_LOGS_FILE_NUM.
 * 
 * When the file exceedes max size, system opens new file logs data to new file.
 * When maximum number of files is reached, system deletes the oldest file and
 * creates new one.
 */

#include "./system/definitions.h"
#include "./libs_3rd_party/micro-sdcard/mySD.h"
#include "./system/utils.h"
#include "./hmi/commands_engine.h"
#include "./system/system_tasks.h"
#include "./libs_3rd_party/LinkedList/LinkedList.h"

static LinkedList<String> logsQueue = LinkedList<String>();
FileSd SystemFile;

/* ID of first (oldest) file */
int FirstSysLogFileId = -1;
/* ID of last (newest) file */
int LastSysLogFileId = -1;

/* ID of file used for searching. This is used when user is searching
    (in cooperation with "./hmi/commands_engine.h") for data via serial port */
int SearchSysLogFileId = -1;


/**
 * @brief   Find newest(LastSysLogFileId) and oldest(FirstSysLogFileId) system file.
 *          If directory doesn't exist, system will create directory for system logs.
 *          If there is any non system logs file in directory it will be deleted.
 * @return  0 - OK
 *          else - ERROR (not implemented)
 */
int InitSystemLogs( void)
{
    /* Logs are disabled if SD card is not initialized */
    if(!systemStat.fsInitialized){
        return 0; 
    }
    /* First run - determine in which file to fill data
        and which file is first for search */
    if (FirstSysLogFileId == -1)
    {
        if (!SD.exists((char *)SYSTEM_LOG_DIR))
        {
            /* Directory does not exist - Create it */
            SD.mkdir((char *)SYSTEM_LOG_DIR);
            FirstSysLogFileId = 0;
            LastSysLogFileId = 0;
        }
        else
        {
            FileSd Directory = SD.open(SYSTEM_LOG_DIR);

            while (1)
            {
                /* Search for files */
                SystemFile = Directory.openNextFile();

                if (SystemFile)
                {
                    int FileId;
                    /* Check if filename is good and parse ID */
                    if (!sscanf(SystemFile.name(), SYSTEM_FILENAME_TEMPLATE, &FileId))
                    {
                        /* If not -> delete file */
                        char FullFileName[30];
                        sprintf( FullFileName, "%s%s", SYSTEM_LOG_DIR, SystemFile.name());
                        SystemFile.close();
                        SD.remove( FullFileName);
                        continue;
                    }

                    /* Check for min/max */
                    if (LastSysLogFileId == -1)
                    {
                        FirstSysLogFileId = FileId;
                        LastSysLogFileId = FileId;
                    }
                    else if (FileId < FirstSysLogFileId)
                    {
                        FirstSysLogFileId = FileId;
                    }
                    else if (FileId > LastSysLogFileId)
                    {
                        LastSysLogFileId = FileId;
                    }
                }
                /* All files found (or no file found) */
                else
                {
                    if (LastSysLogFileId == -1)
                    {
                        FirstSysLogFileId = 0;
                        LastSysLogFileId = 0;
                    }
                    break;
                }
            }
            Directory.close();
        }
        SearchSysLogFileId = FirstSysLogFileId;
    }
    return 0;
}

/**
 * @brief Writes system logs and takes care that none of them reaches max size.
 * Once log size is at its max system will open second log and start writing
 * data in it, old log will be kept until new log exceeds max size, then system
 * will delete old log and switch writing and so on...
 */
void handleSystemLogs()
{
    /* Do not open file if queue is empty */
    if (logsQueue.size() == 0)
    {
        return;
    }

    /* Open file or create a file if needed */
    bool RepeatBecauseLastFileIsFull = false;
    do
    {
        RepeatBecauseLastFileIsFull = false;
        /* Generate filename to print into */
        char ActiveFileFullPath[30];
        sprintf( ActiveFileFullPath, SYSTEM_FULL_FILEPATH_TEMPLATE, LastSysLogFileId);

        SystemFile = SD.open(ActiveFileFullPath, FILE_WRITE);
        if (SystemFile.size() / (1024*1024) >= MAX_SYSTEM_LOG_FILE_SIZE)
        {
            /* If file is too big - close it and open new one in next iteration */
            SystemFile.close();
            LastSysLogFileId++;
            RepeatBecauseLastFileIsFull = true;

            /* If too many files in folder - delete first file */
            while ((LastSysLogFileId - FirstSysLogFileId +1) > MAX_SYSTEM_LOGS_FILE_NUM)
            {
                char RemoveFileFullPath[30];
                sprintf( RemoveFileFullPath, SYSTEM_FULL_FILEPATH_TEMPLATE, FirstSysLogFileId);
                SD.remove(RemoveFileFullPath);
                FirstSysLogFileId++;
            }
        }
    }
    while (RepeatBecauseLastFileIsFull);
    

    /* Save data to file */
    if (SystemFile)
    {
        while (logsQueue.size() > 0)
        {
            SystemFile.println(logsQueue.shift());
            vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
        }
        SystemFile.flush();
        SystemFile.close();
    }
    else
    {
        systemLog(tERROR, "Failed to write log to SD");
    }
}

/**
 * @brief Adds new system log to queue
 * 
 * @param data string to write into logs
 */
void uWriteToSystemLogs( const char *data)
{
    systemLog( tUSER, data);
}

/**
 * @brief Adds new log to queue
 * 
 * @param log Prepared log address
 */
void createFSlog(char *log)
{
    logsQueue.add(String(log));
    // If Queue is full data will be dropped
}


/**
 * @brief Logs system data
 * 
 * @param type Log type
 * @param message Log message
 */
void systemLog(logType type, const char *message)
{
    const char *typeS = "";
    char Time[TIME_STRING_LENGTH];
    char logBuff[255];

    switch (type)
    {
    case tINFO:
    {
        typeS = "INFO";
        break;
    }
    case tSYSTEM:
    {
        typeS = "SYSTEM";
        break;
    }
    case tERROR:
    {
        typeS = "ERROR";
        break;
    }
    case tWARNING:
    {
        typeS = "WARNING";
        break;
    }
    case tUSER:
    {
        typeS = "USER";
        break;
    }

    default:
        break;
    }

    sprintf(logBuff, "[%s] [%s] - %s", getSystemTimeString(Time), typeS, message);
    PrintDebugInfo(logBuff, type == tSYSTEM);
    
    if(type != tINFO && systemStat.fsInitialized){
        createFSlog(logBuff);
    }
}


/**
 * @brief Print data from log file to serial COM port
 * 
 */
void sendSystemLogsToSerial()
{
    if (IsClearingLogsCommandActive())
    {
        FileSd Directory = SD.open(SYSTEM_LOG_DIR);

        /* Delete all files */
        do
        {
            /* Open files one by one */
            SystemFile = Directory.openNextFile();

            if (SystemFile)
            {
                /* Read file name and delete it */
                char FullFileName[30];
                sprintf( FullFileName, "%s%s", SYSTEM_LOG_DIR, SystemFile.name());
                SystemFile.close();
                SD.remove( FullFileName);

                Serial.print("Removed file:");
                Serial.println(FullFileName);
            }
        }
        while (SystemFile);

        Serial.println("System log erased!");

        /* Recreate FS */
        FirstSysLogFileId = -1;
        LastSysLogFileId = -1;

        /* Reset clearing command */
        ResetClearingLogsCommand();
    }
    else if (IsCommandActive())
    {
        /* Generate filename to read from */
        char ActiveFileFullPath[30];
        sprintf( ActiveFileFullPath, SYSTEM_FULL_FILEPATH_TEMPLATE, SearchSysLogFileId);

        SystemFile = SD.open(ActiveFileFullPath, FILE_READ);
        CheckAndPrintLogFileData( &SystemFile);
        
        if (SystemFile)
        {
            SystemFile.close();
        }
        else
        {
            systemLog(tERROR, "Failed to open file");
        }
    }
}