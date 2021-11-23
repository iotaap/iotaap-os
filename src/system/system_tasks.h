#ifndef __SYSTEM_TASKS_H__
#define __SYSTEM_TASKS_H__

#include <stdint.h>
#include "time.h"

struct sSystemStat
{
    uint64_t bootTimeEpochMs;  // Time when system was booted successfully
    uint64_t epochTimeMs;      // epoch time, updated rarely (real time is calculated with help of epochTimeUpdatedMs, epochTimeMs and millis()
    unsigned long epochTimeUpdatedMs;// system millis() when epoch time is updated
    bool systemTimeSynced;     // True if system time is synced with Server
    bool fsInitialized;        // True if filesystem (SD card) is successfully initialized
    bool fatInitialized;       // True if FAT is successfully initialized
    bool updateInProgress;     // True if update is in progress
    bool updateCheckRequested; // True if check for update has been manually requested
    bool sysCfgExist;
};

extern sSystemStat systemStat;

void createSystemTasks();
void SyncTimeTask(void *parameter);
void InitTime( void);
void SetRestartTime( void);
void UpdateTime( uint64_t epoch_ms);
void SystemStatUpdateRequest( void);

#endif