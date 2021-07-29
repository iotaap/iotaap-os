#ifndef __MQTT_DATADEVICE_BATCH_H__
#define __MQTT_DATADEVICE_BATCH_H__

#include <stddef.h>

int uDeviceCloudPublishParamBatch(const char *name, float value);
int uDeviceCloudPublishParamBatch(const char *name, const char *value);
size_t BatchGetLastData( char *buffer);
int BatchNumOfElements( void);
int BatchFullnessPercent( void);

#endif