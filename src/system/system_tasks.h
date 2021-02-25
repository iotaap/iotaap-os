#ifndef __SYSTEM_TASKS_H__
#define __SYSTEM_TASKS_H__

#include <stdint.h>
#include "time.h"

struct sSystemStat
{
    uint64_t bootTime;         // Time when system was booted successfully
    uint64_t uptime;           // Time since boot
    float batteryPerc;         // Backup battery percentage
    struct tm systemTime;      // Current system time synced with NTP
    int64_t systemTimeMs;      // Current system time Unix in ms
    bool systemTimeSynced;     // True if system time is synced with NTP
    bool fsInitialized;        // True if filesystem (SD card) is successfully initialized
    bool updateInProgress;     // True if update is in progress
    bool updateCheckRequested; // True if check for update has been manually requested
};

extern sSystemStat systemStat;

void createSystemTasks();
void SystemStatusTask(void *parameter);
void SyncNTPtask(void *parameter);
void SystemStatUptimeReset( void);
void SystemStatUpdateRequest( void);

#endif