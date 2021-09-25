#ifndef __SYSTEM_CONFIGURATION_H__
#define __SYSTEM_CONFIGURATION_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include "./libs_3rd_party/ArduinoJson-v6.18.4/ArduinoJson-v6.18.4.h"

void InitSystemConfigDataFromJsonDocument( DynamicJsonDocument ConfigJson);

void SystemSetVersion( const char *Version);
const char *SystemGetFwVersion( void);
const char *SystemGetDeviceId( void);
const char *SystemGetGroupId( void);
const char *SystemGetDeviceToken( void);
const char *SystemGetGroupToken( void);
const char *SystemGetOtaServer( void);
int SystemGetTimezone( void);
int SystemGetTimezoneOffsetMs( void);
int SystemGetTimezoneOffsetS( void);
bool SystemGetAutoUpdateFlag( void);
char *SystemGetCAcertificate( void);
char *SystemNewCAcertificate( int certLen);

#endif