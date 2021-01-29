#include "mqtt_dataUser.h"

#include "./mqtt/mqtt_client.h"
#include "./mqtt/mqtt_subscription.h"


/**
 * @brief Pulishes payload to the topic. Root topic (username) will be automatically added
 * 
 * @param payload - Payload (JSON)
 * @param uTopic - Topic to publish to
 * @return int - Returns 0 if successfull
 */
int uBasicCloudPublish(const char *payload, const char *uTopic)
{
    /* Generate endpoint url and publish */
    char topicChar[256];
    sprintf( topicChar, "/%s/%s", MqttGetUser(), uTopic);
    mqttPublish(topicChar, payload, false);
    
    return 0;
}

/**
 * @brief Subsribe to a specific topic. Root topic (username) will be added automatically
 * 
 * @param uTopic - Topic to subscribe to
 * @return int Returns 0 if successfull
 */
int uBasicSubscribe(const char *uTopic)
{
    /* Generate endpoint url and subscribe */
    char topicChar[256];
    sprintf( topicChar, "/%s/%s", MqttGetUser(), uTopic);
    mqttSubscribe(topicChar);

    return 0;
}

/**
 * @brief Unsubscribe from the specific topic
 * 
 * @param uTopic Topic (without root)
 * @return int Returns 0 if successfull
 */
int uBasicUnsubscribe(const char *uTopic)
{
    /* Generate endpoint url and unsubscribe */
    char topicChar[256];
    sprintf( topicChar, "/%s/%s", MqttGetUser(), uTopic);
    mqttUnsubscribe(topicChar);

    return 0;
}
