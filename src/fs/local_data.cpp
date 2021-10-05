#include "local_data.h"

/**
 * This module saves data to queue (saveDataLocally()), 
 * determines if connected to web.
 * If not connected to web then saves from queue to file system.
 * If connected to web then sends data on.
 * This module will not work without SD card 
 */

#include "./system/definitions.h"
#include "./fs/local_data.h"
#include "./fs/sys_logs_data.h"
#include "./mqtt/mqtt_client.h"
#include "./mqtt/mqtt_dataDevice.h"
#include "./system/system_tasks.h"
#include "./system/system_configuration.h"
#include "./system/utils.h"
#include "./wifi/wifi_module.h"
#include <LinkedList.h>
#include "./libs_3rd_party/micro-sdcard/mySD.h"


/* File to backup data */
static int PushMqttFileNumber = -1;
/* File to get backuped data */
static int PopMqttFileNumber = -1;

/* Flag to signalize to create temporary backup (for reset only) */
static bool WantToReset = false;
FileSd LocalDataFile;

static void FillListFromFile( const char *FilePath);
static void FillFileFromList( const char *FilePath,
                LinkedList<sMqttContainer*> *mqttList, unsigned int MaxFileSize);


/**
 * @brief   Initialize MQTT backup
 * 
 * @details This works only if SD card is present! First fill output list
 *              with data from temp file created just before reset. Also count
 *              backup files and file IDs for later.
 * 
 * @return  0 -> Ok
 *          else -> fail or no FS
 */
int InitMqttBackup( void)
{
    /* Logs are disabled if SD card is not initialized */
    if (!systemStat.fsInitialized)
    {
        return 1;
    }

    /* If no file structure, create and exit */
    if (!SD.exists((char *)BACKUP_DATA_DIR))
    {
        /* Directory does not exist - Create it */
        SD.mkdir((char *)BACKUP_DATA_DIR);
        return 0;
    }

    /* Future backup files */
    FileSd Directory = SD.open(BACKUP_DATA_DIR);
    while (1)
    {
        /* Search for files */
        LocalDataFile = Directory.openNextFile();
        
        if (LocalDataFile)
        {
            int FileId;
            /* Check if filename is good and parse ID */
            if (!sscanf(LocalDataFile.name(), BACKUP_DATA_TEMPLATE, &FileId))
            {
                /* If not -> delete file */
                char FullFileName[30];
                sprintf( FullFileName, "%s%s", BACKUP_DATA_DIR, LocalDataFile.name());
                LocalDataFile.close();
                SD.remove( FullFileName);
                vTaskDelay( 300 / portTICK_PERIOD_MS);
                continue;
            }

            /* Check for min/max */
            if (PushMqttFileNumber == -1)
            {
                PushMqttFileNumber = FileId;
                PopMqttFileNumber = FileId;
            }
            else if (FileId < PopMqttFileNumber)
            {
                PopMqttFileNumber = FileId;
            }
            else if (FileId > PushMqttFileNumber)
            {
                PushMqttFileNumber = FileId;
            }
        }
        /* All files found (or no file found) */
        else
        {
            if (PushMqttFileNumber == -1)
            {
                PushMqttFileNumber = 0;
                PopMqttFileNumber = 0;
            }
            else
            {
                PushMqttFileNumber++;
            }

            char output[50];
            sprintf( output, "Backup file count: %d", PushMqttFileNumber-PopMqttFileNumber);
            systemLog( tSYSTEM, output);

            break;
        }
    }
    Directory.close();
    vTaskDelay( 300 / portTICK_PERIOD_MS);
    
    return 0;
}

/**
 * @brief Handle mqtt list data
 *          - if list is too big -> backup data to file
 *          - if list is small and have backuped data -> fill list from file
 */
#define LOW_MEM_LIST    10
#define HIGH_MEM_LIST   150
void handleLocalMqttMessages( void)
{

    if (!mqttDataMsgs || !systemStat.fsInitialized)
    {
        if (WantToReset)
        {
            SetRestartTime();
            ESP.restart();
        }
        return;
    }

    /* Send data to output buffer */
    if (mqttDataMsgs->size()<LOW_MEM_LIST && MqttIsConnected())
    {
        /* Fill buffer from backup file */
        if (PushMqttFileNumber != PopMqttFileNumber)
        {
            char FullFileName[30];
            sprintf( FullFileName, BACKUP_DATA_PATH, PopMqttFileNumber);

            /* Fill list and destroy backup file */
            FillListFromFile( FullFileName);
            SD.remove( FullFileName);

            PopMqttFileNumber++;

            char output[50];
            sprintf( output, "Backup file sent (%d)", PushMqttFileNumber-PopMqttFileNumber);
            systemLog( tSYSTEM, output);
        }
    }

    /* Backup data to file (memory balancer) */
    if (mqttDataMsgs->size() > HIGH_MEM_LIST)
    {
        char FullFileName[30];
        sprintf( FullFileName, BACKUP_DATA_PATH, PushMqttFileNumber);

        /* Store up to 4096bytes */
        FillFileFromList( FullFileName, mqttDataMsgs, BACKUP_FILE_SIZE);
        PushMqttFileNumber++;

        char output[50];
        sprintf( output, "Backup file created (%d)", PushMqttFileNumber-PopMqttFileNumber);
        systemLog( tSYSTEM, output);
        
        vTaskDelay( 100 / portTICK_PERIOD_MS);
    }


    /* Backup all data (reset is coming) */
    if (WantToReset)
    {
        stopPublishing();

        while (mqttDataMsgs->size())
        {
            char FullFileName[30];
            sprintf( FullFileName, BACKUP_DATA_PATH, PushMqttFileNumber);
            FillFileFromList( FullFileName, mqttDataMsgs, BACKUP_FILE_SIZE);
            PushMqttFileNumber++;
            
            char output[50];
            sprintf( output, "Backup file created (%d)", PushMqttFileNumber-PopMqttFileNumber);
            systemLog( tSYSTEM, output);
        }

        vTaskDelay( 1000);

        SetRestartTime();
        ESP.restart();
    }
}


/**
 * @brief   Set flag to backup data and reset device
 */
void backupDataAndRestart( void)
{
    WantToReset = true;
}


/**
 * @brief From selected file fill list and remove file
 */
static void FillListFromFile( const char *FilePath)
{
    /* If file exist, copy messages to list */
    LocalDataFile = SD.open( FilePath, FILE_READ);
    if (LocalDataFile && LocalDataFile.size()<BACKUP_FILE_SIZE)
    {
        int msgs = 0;
        char Data[1024];
        
        /* Take all from file and fill list */
        while (LocalDataFile.readBytesUntil('\n', Data, 1024))
        {
            char *topicEnd = strchr( Data, ' ');
            char *payload = topicEnd+1;
            char *topic = Data;
            *topicEnd = '\0';
            msgs++;

            /* Publish, but to output! */
            mqttPublish( topic, payload, false);
        }

        /* Destroy file */
        LocalDataFile.close();
        SD.remove( (char *)FilePath);
    }
}


/**
 * @brief Backup data from list to file
 */
static void FillFileFromList( const char *FilePath,
                LinkedList<sMqttContainer*> *mqttList, unsigned int MaxFileSize)
{
    /* If temp.log file exist, copy messages to list */
    SD.remove( (char *)FilePath);
    LocalDataFile = SD.open( FilePath, FILE_WRITE);
    if (LocalDataFile)
    {
        int msgs = 0;

        while (mqttList->size())
        {
            char buffer[PAR_JSON_MAX_LEN];
            sMqttContainer *cont = mqttList->get(0);
            struct sMqttData *data = cont->data;
            char *payload = data->payload;

            unsigned int FileSize = LocalDataFile.size();
            bool UseBuffer = FromContCreateJson( cont, buffer);
            unsigned int Len = strlen(cont->topic)+3+ (UseBuffer ? strlen(buffer) : strlen(cont->data->payload));

            /* Check file length */
            if (Len+FileSize > MaxFileSize)
            {
                break;
            }

            /* Fill data */
            LocalDataFile.print( cont->topic);
            LocalDataFile.print( " ");
            LocalDataFile.println( UseBuffer ? buffer : payload);

            /* Destroy data */
            if (!UseBuffer)
            {
                delete[] payload;
            }
            else
            {
                delete[] data->name;
                if (data->flags & 0x02)
                {
                    delete[] data->text;
                }
            }
            
            delete data;
            mqttList->shift();
            delete cont;
            
            msgs++;
        }

        LocalDataFile.flush();
        LocalDataFile.close();
    }
}


/**
 * @brief   From mqtt container create JSON and save it into buffer
 * 
 * @return  true  - use buffer
 *          false - use cont->data->payload
 */
bool FromContCreateJson( struct sMqttContainer *cont, char *buffer)
{
    struct sMqttData *data = cont->data;

    if (data->flags & (1<<2))
    {
        return false;
    }
    else
    {
        /* Create JSON */
        DynamicJsonDocument paramPublishDoc(PAR_JSON_MAX_LEN);
        char Time[TIME_STRING_LENGTH];

        paramPublishDoc.clear();
        paramPublishDoc["device_id"] = SystemGetDeviceId();
        paramPublishDoc["name"] = data->name;
        paramPublishDoc["time"] = getSystemTimeString( Time, (data->unix_ms)/1000);
        paramPublishDoc["unix_ms"] = data->unix_ms;

        if (data->flags & 0x02)
        {
            paramPublishDoc["value"] = data->text;
        }
        else
        {
            paramPublishDoc["value"] = data->value;
        }

        serializeJson( paramPublishDoc, buffer, PAR_JSON_MAX_LEN);
        return true;
    }
}