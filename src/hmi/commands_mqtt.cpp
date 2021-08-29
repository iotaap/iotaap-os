#include "commands_mqtt.h"

#include "./system/definitions.h"
#include "string.h"
#include "stdint.h"
#include "./fs/sys_logs_data.h"
#include "commands_engine.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "./mqtt/mqtt_client.h"
#include "./mqtt/mqtt_remoteControl.h"
#include "./system/system_configuration.h"
#include "./system/utils.h"

#define MAX_JSON_RESPONSE_SIZE  1024
#define HTTP_OVERHEAD           100


/* JSON 1.5 kb - MAX allowed is 1024 */
DynamicJsonDocument* MqttResponseJson;
JsonArray MqttDataArray;


/**
 * @brief Task for detecting and parsing serial inputDebugString
 */
void MqttCommandHandler( char *Json, int Length)
{
    DynamicJsonDocument MqttCommandJson(Length*2);
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
    MqttResponseJson = new DynamicJsonDocument(1024);


// Serial.println("MqttCommandStartResponse");

    MqttResponseJson->clear();
    
    (*MqttResponseJson)["device"] = SystemGetDeviceId();
    (*MqttResponseJson)["command"] = Command;
    (*MqttResponseJson)["time"] = getSystemTimeString( Time);
    (*MqttResponseJson)["part"] = 0;
}

/**
 * @brief Set response element in JSON
 */
void MqttCommandSetResponse( char *Response)
{
// Serial.println("MqttCommandSetResponse");
    (*MqttResponseJson)["response"] = Response;
}


/**
 * @brief Add data to array. If data exceeds given memory, send data and create new JSON
 */
void MqttCommandAddData( char *Data)
{
// Serial.println("MqttCommandAddData");
    /* Check if array exist - create if needed */
    if (!MqttResponseJson->containsKey("data"))
    {
        MqttDataArray = MqttResponseJson->createNestedArray("data");
    }

    /* Not enough space - send data and clear data from json,  */
    if ((measureJson( *MqttResponseJson) + strlen(Data) + HTTP_OVERHEAD) >
                                                        MAX_JSON_RESPONSE_SIZE)
    {
        MqttCommandSendResponse();

        /* Recreate new JSON without data[] */
        char Command[MaxCommandLen];
        int Part = (*MqttResponseJson)["part"];
        strcpy( Command, (*MqttResponseJson)["command"]);

        MqttCommandStartResponse( Command);
        (*MqttResponseJson)["part"] = Part + 1;

        /* Add data */
        MqttCommandAddData( Data);
    }
    else
    {
        MqttDataArray.add( Data);
    }
}


/**
 * @brief Send created JSON
 */

void MqttCommandSendResponse( void)
{
    char OutputData[MAX_JSON_RESPONSE_SIZE];
    serializeJson( *MqttResponseJson, OutputData);

// Serial.print("JSON: ");
// Serial.println(OutputData);

    MqttRespondToRq( OutputData);
    delete MqttResponseJson;
}


/**
 * @brief Send status
 */
void MqttCommandSendStatus( char *Status)
{
//Serial.println("MqttCommandSendStatus");
}