#include "mqtt_systemLogs.h"

#include "./mqtt/mqtt_subscription.h"
#include "./mqtt/mqtt_client.h"
#include "./fs/sys_cfg.h"
#include "./system/system_configuration.h"
#include "./hmi/commands_mqtt.h"



static char SystemReqTopic[64];
static char SystemRespTopic[64];

/**
 * @brief Subsribe to a system req topic.
 */
void mqttSubscribeToSystemRequestTopic( void)
{
    /* Generate endpoint for system requests */
    sprintf( SystemReqTopic, "/%s/devices/%s/logs-req", MqttGetUser(), SystemGetDeviceId());
    mqttSubscribe(SystemReqTopic);

    /* Generate endpoint for system responses */
    sprintf( SystemRespTopic, "/%s/devices/%s/logs-resp", MqttGetUser(), SystemGetDeviceId());
}


/**
 * @brief This function is called when data comes to subscribed topics.
 *        Function choose if this is system request or data for user.
 */
void MqttCallback(char *topic, unsigned char *message, unsigned int length)
{
    /* todo - If request for system logs */
    if (!strcmp( SystemReqTopic, topic))
    {
        MqttCommandHandler( (char *)message, length);
    }
    else
    {
        MqttRunUserCallback( topic, message, length);
    } 
}

/**
 * @brief Send respond to request
 */
void MqttRespondToRq( char *Data)
{
    mqttPublish(SystemRespTopic, Data, false);
}
