#ifndef __SYSTEM_UTILS_H__
#define __SYSTEM_UTILS_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "stdint.h"

float getBatteryPercentage();
unsigned long getAdcValue(int pin);
float getVoltage(unsigned long reading);
char * getSystemTimeString( char *Timestamp);
uint64_t getSystemTimeMs();
void periphInit();

#endif