#ifndef __LED_TASK_H__
#define __LED_TASK_H__

#include "stdint.h"

/* Minimum time between toggling the led */
#define LED_TASK_PERIOD_MS  100

/* Init */
void LedPinInit(unsigned int PinNum);

/* Task */
void LedTask(void *parameter);

/* Usage */
void LedSwitchOn(void);
void LedSwitchOff(void);
void LedBlinkFast(void);
void LedBlinkConfigurator(void);
void LedBlinkSlow(void);
void LedBlinkPeriodic(void);
void LedBlinkUser(unsigned int HighPeriodMs, unsigned int LowPeriodMs);

#endif // __LED_TASK_H__
