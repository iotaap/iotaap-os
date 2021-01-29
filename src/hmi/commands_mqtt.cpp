#include "commands_mqtt.h"

#include "string.h"
#include "stdint.h"
#include "./fs/sys_logs_data.h"
#include "commands_engine.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "./mqtt/mqtt_client.h"
#include "./mqtt/mqtt_systemLogs.h"
#include "./system/system_configuration.h"
#include "./system/utils.h"
#include "./system/definitions.h"

#define MAX_JSON_RESPONSE_SIZE  1024
#define HTTP_OVERHEAD           100


/* JSON 1.5 kb - MAX allowed is 1024 */
StaticJsonDocument<1300> MqttCommandJson;
JsonArray MqttDataArray;


/**
 * @brief Task for detecting and parsing serial inputDebugString
 */
void MqttCommandHandler( char *Json, int Length)
{
    char Input[MaxCommandLen];
    char DeviceId[50];

    Json[Length] = '\0';
//Serial.println("Command arrived!");

    /* Incoming JSON */
    deserializeJson( MqttCommandJson, Json, Length);
    strncpy( Input,    MqttCommandJson["command"], sizeof(Input));
    strncpy( DeviceId, MqttCommandJson["device"],  sizeof(DeviceId));

    /* Outgoing JSON */
//Serial.print("command_: ");
//Serial.println(Input);
    MqttCommandStartResponse( Input);

    if (strcmp( DeviceId, SystemGetDeviceId()))
    {
        char Data[100];
        sprintf( Data, "Wrong device ID! (%s)", DeviceId);
        MqttCommandSetResponse( Data);
        MqttCommandSendResponse();
        return;
    }

//Serial.println( Input);

    ProcessCommand( Input, CommandSourceMqtt);
}

    

/**
 * @brief Clear data in JSON
 */
void MqttCommandStartResponse( char* Command)
{
    char Time[TIME_STRING_LENGTH];

// Serial.println("MqttCommandStartResponse");
    MqttCommandJson.clear();
    
    MqttCommandJson["device"] = SystemGetDeviceId();
    MqttCommandJson["command"] = Command;
    MqttCommandJson["time"] = getSystemTimeString( Time);
    MqttCommandJson["part"] = 0;
}

/**
 * @brief Set response element in JSON
 */
void MqttCommandSetResponse( char *Response)
{
// Serial.println("MqttCommandSetResponse");
    MqttCommandJson["response"] = Response;
}


/**
 * @brief Add data to array. If data exceed given memory, send data and create new json
 */
void MqttCommandAddData( char *Data)
{
// Serial.println("MqttCommandAddData");
    /* Check if array exist - create if needed */
    if (!MqttCommandJson.containsKey("data"))
    {
        MqttDataArray = MqttCommandJson.createNestedArray("data");
    }

    /* Not enough space - send data and clear data from json,  */
    if ((measureJson( MqttCommandJson) + strlen(Data) + HTTP_OVERHEAD) >
                                                        MAX_JSON_RESPONSE_SIZE)
    {
        MqttCommandSendResponse();

        /* Rectreate new JSON without data[] */
        char Command[MaxCommandLen];
        int Part = MqttCommandJson["part"];
        strcpy( Command, MqttCommandJson["command"]);

        MqttCommandStartResponse( Command);
        MqttCommandJson["part"] = Part + 1;

        /* Add data */
        MqttCommandAddData( Data);
    }
    else
    {
        MqttDataArray.add( Data);
    }
}


/**
 * @brief Send crreated JSON
 */

void MqttCommandSendResponse( void)
{
    char OutputData[MAX_JSON_RESPONSE_SIZE];
    serializeJson( MqttCommandJson, OutputData);

// Serial.print("JSON: ");
// Serial.println(OutputData);

    MqttRespondToRq( OutputData);
}


/**
 * @brief Send status
 */
void MqttCommandSendStatus( char *Status)
{
//Serial.println("MqttCommandSendStatus");
}