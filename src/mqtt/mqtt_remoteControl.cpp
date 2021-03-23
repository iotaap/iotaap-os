#include "mqtt_remoteControl.h"

#include "./mqtt/mqtt_subscription.h"
#include "./mqtt/mqtt_client.h"
#include "./fs/sys_cfg.h"
#include "./system/system_configuration.h"
#include "./hmi/commands_mqtt.h"
#include "./system/system_tasks.h"



static char SystemReqTopic[64];
static char SystemRespTopic[64];

static char RemoteUpdateReqTopic[64];
static char RemoteUpdateRespTopic[64];

/**
 * @brief Subscribe to remote control topics
 */
void mqttSubscribeToRemoteControlTopics( void)
{
    /* Generate endpoint for remote requests */
    sprintf( SystemReqTopic, "/%s/devices/%s/logs-req", MqttGetUser(), SystemGetDeviceId());
    mqttSubscribe(SystemReqTopic);

    sprintf( RemoteUpdateReqTopic, "/%s/devices/%s/update-trigger", MqttGetUser(), SystemGetDeviceId());
    mqttSubscribe(RemoteUpdateReqTopic);

    /* Generate endpoint for remote responses */
    sprintf( SystemRespTopic, "/%s/devices/%s/logs-resp", MqttGetUser(), SystemGetDeviceId());
    sprintf( RemoteUpdateRespTopic, "/%s/devices/%s/update-response", MqttGetUser(), SystemGetDeviceId());
}


/**
 * @brief This function is called when data comes to subscribed topics.
 *        Function chooses if this data is a remote request or for user.
 */
void MqttCallback(char *topic, unsigned char *message, unsigned int length)
{
    /* If request is for remote or user*/
    if (!strcmp( SystemReqTopic, topic))
    {
        MqttCommandHandler( (char *)message, length);
    } 
    else if(!strcmp( RemoteUpdateReqTopic, topic)){
        SystemStatUpdateRequest();
    }
    else
    {
        MqttRunUserCallback( topic, message, length);
    } 
}

/**
 * @brief Send response to system request
 */
void MqttRespondToRq( char *Data)
{
    mqttPublish(SystemRespTopic, Data, false);
}

/**
 * @brief Send response to update request
 */
void MqttRespondToUpdateRequest( char *Data)
{
    mqttPublish(RemoteUpdateRespTopic, Data, false);
    /** Force publishing message, to be kinda sure that cloud will get the response that request has been received **/
    triggerMqttPublish_Extern(); 
}