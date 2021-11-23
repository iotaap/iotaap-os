#ifndef __WIFI_MODULE_H__
#define __WIFI_MODULE_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include "./libs_3rd_party/ArduinoJson-v6.18.4/ArduinoJson-v6.18.4.h"

void wifiConnect( void);
void InitWificonfigDataFromJsonDocument( DynamicJsonDocument ConfigJson);
bool WifiCredentialsExist( void);

bool WifiIsConnected( void);

#endif