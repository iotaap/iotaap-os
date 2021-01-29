#ifndef __SYSTEM_UTILS_H__
#define __SYSTEM_UTILS_H__

#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"
#include "stdint.h"

float getBatteryPercentage();
unsigned long getAdcValue(int pin);
float getVoltage(unsigned long reading);
char * getSystemTimeString( char *Timestamp);
uint64_t getSystemTimeMs();
void periphInit();

#endif