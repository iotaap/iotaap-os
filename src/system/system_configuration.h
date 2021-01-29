#ifndef __SYSTEM_CONFIGURATION_H__
#define __SYSTEM_CONFIGURATION_H__

#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"

void InitSystemConfigDataFromJsonDocument( DynamicJsonDocument ConfigJson);

void SystemSetVersion( const char *Version);
const char *SystemGetFwVersion( void);
const char *SystemGetDeviceId( void);
const char *SystemGetGroupId( void);
const char *SystemGetDeviceToken( void);
const char *SystemGetGroupToken( void);
const char *SystemGetNtp1( void);
const char *SystemGetNtp2( void);
int SystemGetTimezone( void);
bool SystemGetAutoUpdateFlag( void);
char *SystemGetCAcertificate( void);

#endif