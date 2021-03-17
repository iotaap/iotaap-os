#include "system_json.h"
#include "./configurator/configurator.h"
#include "./system/system_tasks.h"

struct sSystemJsonAllData
{
    struct sJsonKeys *JsonData;
    int StructSize;
};

static struct sJsonKeys *FindElementKey( const char *Element);

/* Structure to store data from initialized JSON */
struct sSystemJsonAllData SystemJsonAllData[5] = {0};
unsigned int SystemJsonAllDataLength = 0;

/**
 * @brief Parse data from JSON to system structures
 */
void InitDataFromSystemJson( DynamicJsonDocument ConfigJson,
                        struct sJsonKeys *JsonSystemData, int JsonStructSize)
{
    /* Register JsonSystemData to configurator web page */
    configuratorRegister( JsonSystemData, JsonStructSize);

    if (!systemStat.sysCfgExist)
    {
        return;
    }
    
    /* Save to locally to have data, once will be requested */
    SystemJsonAllData[SystemJsonAllDataLength].JsonData   = JsonSystemData;
    SystemJsonAllData[SystemJsonAllDataLength].StructSize = JsonStructSize;
    SystemJsonAllDataLength++;

    for (int i=0; i<JsonStructSize; i++)
    {
        /* String length - also and a flag if it is a string */
        int KeyValueLength = 0;

        switch (JsonSystemData[i].ElementDataType)
        {
            case JsonDataTypeInt:
            {
                *(int *)JsonSystemData[i].ElementPointer =
                                ConfigJson[JsonSystemData[i].ElementKey];
            } break;

            case JsonDataTypeBool:
            {
                *(bool *)JsonSystemData[i].ElementPointer =
                                ConfigJson[JsonSystemData[i].ElementKey];
            } break;

            case JsonDataTypeString_20:
            case JsonDataTypePass_20:   KeyValueLength=20; break;
            case JsonDataTypeString_30:
            case JsonDataTypePass_30:   KeyValueLength=30; break;
            case JsonDataTypeString_32:
            case JsonDataTypePass_32:   KeyValueLength=32; break;

        }

        /* If string - copy string */
        if (KeyValueLength)
        {
            strncpy( (char *)JsonSystemData[i].ElementPointer,
                     ConfigJson[JsonSystemData[i].ElementKey],
                     KeyValueLength);
        }
    }
}


/**
 * @brief Get system parameters
 * 
 */
bool uGetSystemParameter( const char *Element, char *Output)
{
    struct sJsonKeys *JsonKey = FindElementKey( Element);

    if (JsonKey)
    {
        strcpy(Output, (char *)JsonKey->ElementPointer);
        return true;
    }
    return false;
}
bool uGetSystemParameter( const char *Element, int *Output)
{
    struct sJsonKeys *JsonKey = FindElementKey( Element);

    if (JsonKey)
    {
        *Output = *(int *)(JsonKey->ElementPointer);
        return true;
    }
    return false;
}
bool uGetSystemParameter( const char *Element, bool *Output)
{
    struct sJsonKeys *JsonKey = FindElementKey( Element);

    if (JsonKey)
    {
        *Output = *((bool *)JsonKey->ElementPointer);
        return true;
    }
    return false;
}

/**
 * @brief   Find element key
 * @return  Pointer to element key or NULL if not found
 */
static struct sJsonKeys *FindElementKey( const char *Element)
{
    for (int i=0; i<SystemJsonAllDataLength; i++)
    {
        for (int j=0; j<SystemJsonAllData[i].StructSize; j++)
        {
            struct sJsonKeys *LocalData = &SystemJsonAllData[i].JsonData[j];
            if ((strlen( Element) == strlen( LocalData->ElementKey)) &&
                !strcmp( Element, LocalData->ElementKey))
            {
                return LocalData;
            }
        }
    }

    return NULL;
}
