#ifndef __MQTT_DATAUSER_H__
#define __MQTT_DATAUSER_H__


int uBasicUnsubscribe(const char *uTopic);
int uBasicSubscribe(const char *uTopic);
int uBasicCloudPublish(const char *payload, const char *uTopic);
int uCustomPublish(const char *payload, const char *uTopic);
int uCustomSubscribe(const char *uTopic);
int uCustomUnsubscribe(const char *uTopic);

#endif