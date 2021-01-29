#ifndef __MQTT_DATAUSER_H__
#define __MQTT_DATAUSER_H__


int uBasicUnsubscribe(const char *uTopic);
int uBasicSubscribe(const char *uTopic);
int uBasicCloudPublish(const char *payload, const char *uTopic);

#endif