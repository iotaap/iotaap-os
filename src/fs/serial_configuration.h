#ifndef __SERIAL_CONFIGURATION_H__
#define __SERIAL_CONFIGURATION_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include "stdint.h"

void SerialJsonCfgSelectFile( const char *FilePath);
bool SerialJsonCfgPrintKey( void);
void SerialJsonCfgSetValue( char *Data);
void HandleJsonCfgFile( void);
bool FromCfgSaveData( bool ResetAfter);

#endif // __SERIAL_CONFIGURATION_H__
