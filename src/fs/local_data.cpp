#include "local_data.h"

/**
 * This module saves data to queue (saveDataLocally()), 
 * determines if connected to web.
 * If not connected to web then saves from queue to file system.
 * If connected to web then sends data on.
 */

#include "./system/definitions.h"
#include "./libs_3rd_party/micro-sdcard/mySD.h"
#include "./system/queue.h"
#include "./fs/sys_logs_data.h"
#include "./mqtt/mqtt_client.h"
#include "./mqtt/mqtt_dataDevice.h"
#include "./system/system_tasks.h"

Queue<String> localDataQueue(LOCAL_DATA_QUEUE_SIZE);
FileSd LocalDataFile;

/**
 * @brief Saves payload locally if there is no Cloud connection - adds data to
 *          queue which will be handled by FSmanager
 * 
 * @param payload 
 */
void saveDataLocally(const char *payload)
{
    if(systemStat.fsInitialized){
        localDataQueue.push(String(payload));
        // If Queue is full data will be dropped
    }
}

/**
 * @brief Stores data locally from local queue
 * 
 */
void handleAndPublishLocalData()
{
    /**
     * If system is connected and file doesn't exist
     *      -> send data from queue to web
     */
    if (MqttIsConnected() && !SD.exists((char *)LOCAL_DATA_PATH))
    {
        while (MqttIsConnected() && localDataQueue.count() > 0)
        {
            uDeviceCloudPublish(localDataQueue.pop().c_str(), "params");

            systemLog(tINFO, "Data stored locally!");
            vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
        }
    }

    /**
     * System is not connected to web or file with data exists
     *      -> Sends queue to file and file to web
     */
    else
    {
        /* Open file */
        LocalDataFile = SD.open(LOCAL_DATA_PATH, FILE_WRITE);

        /* Send from queue to file 
         *  - System is in a state without connection (save to file) OR
         *  - System is in a state with connection but with data in file. In this
         *    case data to web must go in right order, so first save data to
         *    end of file. Later this data will be sent in right order.
         */
        if (LocalDataFile)
        {
            while (localDataQueue.count() > 0)
            {
                LocalDataFile.println(localDataQueue.pop());

                systemLog(tINFO, "Data stored locally!");
                vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
            }
            LocalDataFile.flush();
        }
        else
        {
            systemLog(tERROR, "Failed to write data to SD");
            return;
        }

        /* Send data from file to web in right order and delete file */
        if (MqttIsConnected())
        {
            char lineFromLocalData[256];
            bool infoLogged = false;

            while (LocalDataFile.available())
            {
                if (!infoLogged)
                {
                    systemLog(tINFO, "Publishing local data");
                    infoLogged = true;
                }
                LocalDataFile.readBytesUntil('\n', lineFromLocalData, 256);
                uDeviceCloudPublish(lineFromLocalData, "params");
                vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
            }
            LocalDataFile.close();
            SD.remove((char *)LOCAL_DATA_PATH);
        }
        else
        {
            LocalDataFile.close();
        }
    }
}
