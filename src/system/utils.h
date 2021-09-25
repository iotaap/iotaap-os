#ifndef __SYSTEM_UTILS_H__
#define __SYSTEM_UTILS_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include "./libs_3rd_party/ArduinoJson-v6.18.4/ArduinoJson-v6.18.4.h"
#include "stdint.h"

float getBatteryPercentage();
unsigned long getAdcValue(int pin);
float getVoltage(unsigned long reading);
char * getSystemTimeString( char *Timestamp);
char *getSystemTimeString( char *Timestamp, time_t unix_s);
uint64_t getSystemUptimeS( void);
uint64_t getUnixTimeMs();
void periphInit();
void createSSID( char *ssid);

#endif