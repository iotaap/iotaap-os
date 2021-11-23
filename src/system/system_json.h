#ifndef __SYSTEM_JSON_H__
#define __SYSTEM_JSON_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include "./libs_3rd_party/ArduinoJson-v6.18.4/ArduinoJson-v6.18.4.h"

/**
 * Data types used in JSON configuration file
 * New enumerants should be also added to InitDataFromSystemJson() function
 */
enum eJsonDataType
{
    JsonDataTypeInt,
    JsonDataTypeBool,
    JsonDataTypeString,
    JsonDataTypePass,
};

struct sJsonKeys
{
    union
    {
        void *ElementPointer;
        void **ElDoublePointer;
    };
    enum eJsonDataType ElementDataType;
    const char *ElementKey;
    const char *ElementDesc;
    const char *Block_IngEd;    /* Blocking some div or blocked by dome div */
};

void InitDataFromSystemJson( DynamicJsonDocument ConfigJson,
                        struct sJsonKeys *JsonSystemData, int JsonStructSize);
bool uGetSystemParameter( const char *Element, char *Output);
bool uGetSystemParameter( const char *Element, int *Output);
bool uGetSystemParameter( const char *Element, bool *Output);

#endif /* __SYSTEM_JSON_H__ */
