#ifndef __MQTT_SYSTEMLOGS_H__
#define __MQTT_SYSTEMLOGS_H__

void mqttSubscribeToSystemRequestTopic( void);
void MqttCallback(char *topic, unsigned char *message, unsigned int length);
void MqttRespondToRq( char *Data);

#endif