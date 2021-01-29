#ifndef __COMMANDS_SERIAL_H__
#define __COMMANDS_SERIAL_H__

void SerialTask(void *parameter);

void SerialCommandSetResponse( char *Response);
void SerialCommandAddData( char *Data);
void SerialCommandSendResponse( void);
void SerialCommandSendStatus( char *Status);

#endif