#ifndef __SYS_LOGS_H__
#define __SYS_LOGS_H__

/* LOG TYPES */
enum logType
{
    tINFO = 0,
    tERROR,
    tWARNING,
    tUSER
};

int InitSystemLogs( void);
void handleSystemLogs();
void createFSlog(char *log);
void uWriteToSystemLogs( const char *data);
void systemLog(logType type, const char *message);
void sendSystemLogsToSerial();

extern int FirstSysLogFileId;
extern int LastSysLogFileId;
extern int SearchSysLogFileId;

#endif