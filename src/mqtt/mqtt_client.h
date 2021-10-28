#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#define ARDUINOJSON_USE_LONG_LONG 1 // Using int64 variables in JSON

#include <WiFiClientSecure.h>
#include "./libs_3rd_party/pubsubclient-2.8/src/PubSubClient.h"
#include "./libs_3rd_party/ArduinoJson-v6.18.4/ArduinoJson-v6.18.4.h"
#include "./libs_3rd_party/LinkedList/LinkedList.h"
#include <list>
using namespace std;

/**
 * Structure for MQTT message
 */
struct sMqttData
{
    union
    {
        float value;
        char *text;
        char *payload;
    };
    int64_t unix_ms;
    char *name;
    uint8_t flags; /* bit0-retain, bit1-(0-float, 1-char), bit2-payload */
};
struct sMqttContainer
{
    char *topic;
    sMqttData *data;
};

extern LinkedList<sMqttContainer*> *mqttDataMsgs;
extern WiFiClient *wifiClient;
extern PubSubClient *_mqttClient;

void connectToMqtt( void);
void InitMqttconfigDataFromJsonDocument( DynamicJsonDocument ConfigJson);
void mqttPublish( const char *topic, const char *name, const char *value, boolean retain);
void mqttPublish( const char *topic, const char *name, float value, boolean retain);
void mqttPublish( const char *topic, const char *payload, boolean retain);
void triggerMqttPublish_Extern();

bool MqttIsConnected( void);
void MqttSetCallback( MQTT_CALLBACK_SIGNATURE);
const char *MqttGetUser( void);
void MqttRunUserCallback( char *Topic, uint8_t *Message, unsigned int Length);
void stopPublishing( void);

#endif