#ifndef __MQTT_DATADEVICE_BATCH_H__
#define __MQTT_DATADEVICE_BATCH_H__

#include <stddef.h>

bool InitBatchData( int size=2048, int periodMs=200);
int getBatchDiffPeriod( void);
int uDeviceCloudPublishParamBatch(const char *name, float value);
int uDeviceCloudPublishParamBatch(const char *name, const char *value);
size_t BatchGetLastData( char *buffer);
int BatchNumOfElements( void);
int BatchFullnessPercent( void);

#endif