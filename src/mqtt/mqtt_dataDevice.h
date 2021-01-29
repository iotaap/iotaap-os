#ifndef __MQTT_DATADEVICE_H__
#define __MQTT_DATADEVICE_H__

int uDeviceCloudPublish(const char *payload, const char *uTopic);

int uDeviceCloudPublishParam(const char *name, float value);
int uDeviceCloudPublishParam(const char *name, const char *value);

#endif