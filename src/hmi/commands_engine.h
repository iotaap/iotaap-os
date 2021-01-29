#ifndef __COMMANDS_ENGINE_H__
#define __COMMANDS_ENGINE_H__

#include "./libs_3rd_party/micro-sdcard/mySD.h"

/* Maximum command length */
extern const unsigned int MaxCommandLen;

enum eCommandSource
{
    CommandSourceMqtt,
    CommandSourceSerial,
};

void ProcessCommand( const char *Command, enum eCommandSource Source);
void PrintDebugInfo( const char* DebugString);

void CheckAndPrintLogFileData( File *OpenedLogFile);
bool IsCommandActive( void);

bool IsClearingLogsCommandActive( void);
void ResetClearingLogsCommand( void);

bool ValidateTime( char *TimeString);

#endif
