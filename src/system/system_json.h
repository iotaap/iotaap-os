#ifndef __SYSTEM_JSON_H__
#define __SYSTEM_JSON_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"

/**
 * Data types used in JSON configuration file
 * New enumerants should be also added to InitDataFromSystemJson() function
 */
enum eJsonDataType
{
    JsonDataTypeInt,
    JsonDataTypeBool,
    JsonDataTypeString_20,
    JsonDataTypeString_30,
    JsonDataTypeString_32,
};

struct sJsonKeys
{
    void *ElementPointer;
    enum eJsonDataType ElementDataType;
    const char *ElementKey;
};

void InitDataFromSystemJson( DynamicJsonDocument ConfigJson,
                        struct sJsonKeys *JsonSystemData, int JsonStructSize);
bool uGetSystemParameter( const char *Element, char *Output);
bool uGetSystemParameter( const char *Element, int *Output);
bool uGetSystemParameter( const char *Element, bool *Output);

#endif /* __SYSTEM_JSON_H__ */
