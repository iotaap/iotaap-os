#include "mqtt_dataDeviceBatch.h"

#include "./system/definitions.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "./system/utils.h"
#include "./system/system_tasks.h"
#include "./fs/local_data.h"
#include "./mqtt/mqtt_client.h"
#include "./fs/sys_cfg.h"
#include "./system/system_configuration.h"


void BatchRemoveLastData( void);
int BatchFullnessPercent( void);

/**
 * Structure for MQTT message
 */
struct sMqttData
{
    int64_t unix_ms;
    uint32_t flags; /* bit0-retain, bit1-(0-float, 1-char), bit2-payload */
    float value;
};

/* List for "parName" */
uint8_t batch[BATCH_SIZE] = {0};
uint8_t *bread  = batch;
uint8_t *bwrite = batch;
uint8_t *batchEnd = batch + BATCH_SIZE;
int ElementsCount = 0;

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
// Serial.println();
// Serial.println("Add2: ");
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
    data->unix_ms = systemStat.systemTimeMs;
    data->flags = (false << 0) | (0 << 1);
    data->value = value;

    data++;
    char *parName = (char *)data;

// Serial.println();
// Serial.print("    Perc:");
// Serial.println( BatchFullnessPercent());
// Serial.print("UntilEnd: ");
// Serial.println( SpaceUntilEnd);
// Serial.print("    Name: ");
// Serial.println( name);
// Serial.print("Distance: ");
// Serial.println( bwrite - batch);
// Serial.print(" DataLen: ");
// Serial.println( sizeof( struct sMqttData));
// Serial.print("  Strlen: ");
// Serial.println( nameLen);
// Serial.print("    Full: ");
// Serial.println( ElementSize);
// Serial.print("   Start: ");
// Serial.println( (uintptr_t)bwrite);
// Serial.print("    Name: ");
// Serial.println( (uintptr_t)parName);

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
// Serial.print(" 2EndCnt: ");
// Serial.println( ElementsCount);

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
// Serial.println();
// Serial.println("Add1: ");
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
    data->unix_ms = systemStat.systemTimeMs;
    data->flags = (false << 0) | (1 << 1);

    data++;
    char *parName = (char *)data;
    char *valName = strcpy( parName, name) + nameLen + 1;


// Serial.println();
// Serial.print("    Perc:");
// Serial.println( BatchFullnessPercent());
// Serial.print("UntilEnd: ");
// Serial.println( SpaceUntilEnd);
// Serial.print("Distance: ");
// Serial.println( bwrite - batch);
// Serial.print("    Name: ");
// Serial.println( name);
// Serial.print(" DataLen: ");
// Serial.println( sizeof( struct sMqttData));
// Serial.print("  Strlen: ");
// Serial.println( nameLen);
// Serial.print("  Vallen: ");
// Serial.println( valLen);
// Serial.print("    Full: ");
// Serial.println( ElementSize);
// Serial.print("   Start: ");
// Serial.println( (uintptr_t)bwrite);
// Serial.print("    Name: ");
// Serial.println( (uintptr_t)parName);
// Serial.print("     Val: ");
// Serial.println( (uintptr_t)valName);

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
// Serial.print(" 1EndCnt: ");
// Serial.println( ElementsCount);

    return 0;
}


size_t BatchGetLastData( char *buffer)
{
// Serial.println("GET");
    StaticJsonDocument <256> batcPublishDoc;
    struct sMqttData *data = (struct sMqttData *)bread;
    uint32_t flags = data->flags;
    char Time[TIME_STRING_LENGTH];

    batcPublishDoc.clear();
    batcPublishDoc["time"] = getSystemTimeString( Time, (data->unix_ms)/1000);
    batcPublishDoc["unix_ms"] = data->unix_ms;

    // Serial.println(data->value);
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
// Serial.print(" GETcnt: ");
// Serial.println( ElementsCount);
    return serializeJson(batcPublishDoc, buffer, 256);
}


void BatchRemoveLastData( void)
{
// Serial.println("REMOVE");
    if (!ElementsCount)
    {
// Serial.println("EL is 0 - go out of removing!");
        return;
    }
    struct sMqttData *data = (struct sMqttData *)bread;
    uint32_t flags = data->flags;

// Serial.println();
// Serial.print("   Start: ");
// Serial.println( (uintptr_t)bread);

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
// Serial.print(" RMCnt: ");
// Serial.println( ElementsCount);

// Serial.print("    Next: ");
// Serial.println( (uintptr_t)bread);
}


int BatchNumOfElements( void)
{
    return ElementsCount;
}

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