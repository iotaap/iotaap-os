#ifndef __MQTT_REMOTECONTROL_H__
#define __MQTT_REMOTECONTROL_H__

void mqttSubscribeToRemoteControlTopics( void);
void MqttCallback(char *topic, unsigned char *message, unsigned int length);
void MqttRespondToRq( char *Data);
void MqttRespondToUpdateRequest( char *Data);

#endif