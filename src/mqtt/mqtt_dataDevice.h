#ifndef __MQTT_DATADEVICE_H__
#define __MQTT_DATADEVICE_H__

int uDeviceCloudPublishParam(const char *name, float value);
int uDeviceCloudPublishParam(const char *name, const char *value);
int uDeviceCloudPublish(const char *payload, const char *uTopic);
int uDeviceCloudPublish(const char *uTopic, const char *uName, const char *value);
int uDeviceCloudPublish(const char *uTopic, const char *uName, float value);

#endif