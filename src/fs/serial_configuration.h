#ifndef __SERIAL_CONFIGURATION_H__
#define __SERIAL_CONFIGURATION_H__

#include "stdint.h"

void SerialJsonCfgSelectFile( const char *FilePath);
bool SerialJsonCfgPrintKey( void);
void SerialJsonCfgSetValue( char *Data);
void HandleJsonCfgFile( void);

#endif // __SERIAL_CONFIGURATION_H__
