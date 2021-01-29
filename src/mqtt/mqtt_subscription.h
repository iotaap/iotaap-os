#ifndef __MQTT_SUBSCRIOTION_H__
#define __MQTT_SUBSCRIOTION_H__


void mqttSubscribe(const char *topic);
void mqttUnsubscribe(const char *topic);

void subscribeToTopics();
void unsubscribeFromTopics();
void restoreSubscribedTopics();


#endif