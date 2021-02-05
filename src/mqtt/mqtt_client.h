#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include "./libs_3rd_party/pubsubclient-2.8/src/PubSubClient.h"
#include "./libs_3rd_party/ArduinoJson-v6.14.1/ArduinoJson-v6.14.1.h"

extern PubSubClient _mqttClient;

void connectToMqtt( void);
void InitMqttconfigDataFromJsonDocument( DynamicJsonDocument ConfigJson);
void mqttPublish( const char *topic, const char *payload, boolean retain);

bool MqttIsConnected( void);
void MqttSetCallback( MQTT_CALLBACK_SIGNATURE);
const char *MqttGetUser( void);
void MqttRunUserCallback( char *Topic, uint8_t *Message, unsigned int Length);

#endif