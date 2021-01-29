#ifndef __COMMANDS_MQTT_H__
#define __COMMANDS_MQTT_H__

void MqttCommandHandler( char *Input, int Length);

void MqttCommandSetResponse( char *Response);
void MqttCommandAddData( char *Data);
void MqttCommandSendResponse( void);
void MqttCommandStartResponse( char* Command);
void MqttCommandSendStatus( char *Status);

#endif