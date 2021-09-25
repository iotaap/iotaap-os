#include "mqtt_dataDeviceBatch.h"

#include "./system/definitions.h"
#include "./libs_3rd_party/ArduinoJson-v6.18.4/ArduinoJson-v6.18.4.h"
#include "./system/utils.h"
#include "./system/system_tasks.h"
#include "./fs/local_data.h"
#include "./mqtt/mqtt_client.h"
#include "./fs/sys_cfg.h"
#include "./system/system_configuration.h"

static void BatchRemoveLastData( void);

/**
 * Structure for MQTT message
 */
struct sMqttData
{
    int64_t unix_ms;
    uint32_t flags; /* bit0-retain, bit1-(0-float, 1-char), bit2-payload */
    float value;
};

int BATCH_SIZE = 2048;
int batchTimePeriodMs = 200;
uint8_t *batch = NULL;
uint8_t *bread;
uint8_t *bwrite;
uint8_t *batchEnd;
int ElementsCount = 0;


/**
 * @brief   Initialize batch data memory and sending period
 * @par     size      size for data (~20-30 msgs in 1024bytes)
 * @par     periodMs  sending period (this period can be shorter if lot of data in batch)
 */
bool InitBatchData( int size, int periodMs)
{
    if (batch)
    {
        delete[] batch;
    }

    batch = new uint8_t[size];
    batchTimePeriodMs = periodMs;
    bread  = batch;
    bwrite = batch;
    batchEnd = batch + BATCH_SIZE;
    ElementsCount = 0;

    return (batch > 0);
}

/**
 * @brief   Get batch data sending period
 */
int getBatchDiffPeriod( void)
{
    return batchTimePeriodMs;
}


/**
 * @brief Creates and publishes parameter document to cloud or local
 * 
 * @param name Value name
 * @param value Numeric Value
 * @param time Time
 * @return int - expects 0 return on success
 */
int uDeviceCloudPublishParamBatch(const char *name, float value)
{
    if (!batch)
    {
        if (!InitBatchData())
        {
            return -1;
        }
    }

    /* Length od name */
    int nameLen = strlen(name);
    /* Bytes until end of an array (here we dont check if memory is busy or not */
    int SpaceUntilEnd = batch + BATCH_SIZE - bwrite;
    /* Memory needed to write object */
    int ElementSize = sizeof( struct sMqttData)  + nameLen + 1;
    if (SpaceUntilEnd < ElementSize)
    {
        if (bread > bwrite)
        {
            BatchRemoveLastData();
        }
        batchEnd = bwrite;
        memset( batchEnd, 0, SpaceUntilEnd);
        bwrite = batch;
    }

    /* Destroy old data if we made full circle */
    while (bwrite<=bread && bwrite+ElementSize>bread && ElementsCount)
    {
        BatchRemoveLastData();
    }

    /* Start writing data */
    struct sMqttData *data = (struct sMqttData *)bwrite;
    data->unix_ms = getUnixTimeMs();
    data->flags = (false << 0) | (0 << 1);
    data->value = value;

    data++;
    char *parName = (char *)data;

    bwrite = (uint8_t *)strcpy( parName, name) + nameLen+1;
    while ((uintptr_t)bwrite & 7)
    {
        bwrite++;
    };

    if (bwrite >= batch + BATCH_SIZE)
    {
        bwrite = batch;
    }

    ElementsCount++;

    return 0;
}


/**
 * @brief Creates and publishes parameter document to cloud or local
 * 
 * @param name Value name
 * @param value Non-numeric Value 
 * @param time Time
 * @return int 
 */
int uDeviceCloudPublishParamBatch(const char *name, const char *value)
{
    if (!batch)
    {
        if (!InitBatchData())
        {
            return -1;
        }
    }

    /* Length od name */
    int nameLen = strlen(name);
    /* Length od name */
    int valLen = strlen(value);
    /* Bytes until end of an array (here we dont check if memory is busy or not */
    int SpaceUntilEnd = batch + BATCH_SIZE - bwrite;
    /* Memory needed to write object */
    int ElementSize = sizeof( struct sMqttData)  + nameLen + 1 + valLen + 1;
    if (SpaceUntilEnd < ElementSize)
    {
        if (bread > bwrite)
        {
            BatchRemoveLastData();
        }
        batchEnd = bwrite;
        memset( batchEnd, 0, SpaceUntilEnd);
        bwrite = batch;
    }

    /* Destroy old data if we made full circle */
    while (bwrite<=bread && bwrite+ElementSize>bread && ElementsCount)
    {
        BatchRemoveLastData();
    }

    /* Start writing data */
    struct sMqttData *data = (struct sMqttData *)bwrite;
    data->unix_ms = getUnixTimeMs();
    data->flags = (false << 0) | (1 << 1);

    data++;
    char *parName = (char *)data;
    char *valName = strcpy( parName, name) + nameLen + 1;

    bwrite = (uint8_t *)strcpy( valName, value) + valLen+1;
    while ((uintptr_t)bwrite & 7)
    {
        bwrite++;
    };

    if (bwrite >= batch + BATCH_SIZE)
    {
        bwrite = batch;
    }

    ElementsCount++;

    return 0;
}


/**
 * @brief   Fetch (and remove) latest data in batch memory. Store data into
 *              buffer as JSON
 */
size_t BatchGetLastData( char *buffer)
{
    if (!batch)
    {
        return -1;
    }

    DynamicJsonDocument batcPublishDoc(256);
    struct sMqttData *data = (struct sMqttData *)bread;
    uint32_t flags = data->flags;
    char Time[TIME_STRING_LENGTH];

    batcPublishDoc.clear();
    batcPublishDoc["time"] = getSystemTimeString( Time, (data->unix_ms)/1000);
    batcPublishDoc["unix_ms"] = data->unix_ms;

    if (!(flags & 0x02))
    {
        batcPublishDoc["value"] = data->value;
    }

    data++;
    char *name = (char *)data;
    batcPublishDoc["name"] = name;

    if (flags & 0x02)
    {
        char *text = name + strlen(name)+1;
        batcPublishDoc["value"] = text;
    }

    BatchRemoveLastData();
    return serializeJson(batcPublishDoc, buffer, 256);
}


/**
 * @brief   Get number of stored data in batch memory
 */
int BatchNumOfElements( void)
{
    return ElementsCount;
}


/**
 * @brief   Calculate percent of fullness in batch memory
 */
int BatchFullnessPercent( void)
{
    if (!ElementsCount)
    {
        return 0;
    }
    
    int allocated;
    if (bwrite > bread)
    {
        allocated = bwrite-bread;
    }
    else
    {
        allocated = BATCH_SIZE-(bread-bwrite);
    }

    return (allocated*100)/BATCH_SIZE;
}


/**
 * @brief   Remove oldest data in batch memory
 */
static void BatchRemoveLastData( void)
{
    if (!ElementsCount || !batch)
    {
        return;
    }
    
    struct sMqttData *data = (struct sMqttData *)bread;
    uint32_t flags = data->flags;

    data->unix_ms = 0;
    data->flags   = 0;
    data->value   = 0;
    data++;
    char *name = (char *)data;
    int nLen = strlen(name);
    bread = (uint8_t *)memset( name, 0, nLen) + nLen + 1;

    if (flags & 0x02)
    {
        char *text = name+nLen+1;
        int tLen = strlen(text);
        bread = (uint8_t *)memset( text, 0, tLen) + tLen + 1;
    }

    while ((uintptr_t)bread & 7)
    {
        bread++;
    };

    if (bread >= batchEnd)
    {
        bread = batch;
    }

    ElementsCount--;
    
    if (!ElementsCount && bread==bwrite)
    {
        bread = batch;
        bwrite = batch;
    }
}
