#include "mqtt_subscription.h"

#include "./system/queue.h"
#include "./system/definitions.h"
#include "./fs/sys_logs_data.h"
#include "./mqtt/mqtt_client.h"

/* Max n topics can be added to queue. Once subscribed, topic will be added to subscribed queue */
Queue<String> subsTopicsPendingQueue(SUBS_TOPIC_QUEUE_SIZE);
Queue<String> subsTopicsSubscribedQueue(SUBS_TOPIC_QUEUE_SIZE);
Queue<String> unsubsTopicsQueue(UNSUBS_TOPIC_QUEUE_SIZE);


/**
 * @brief Unsubscribes from selected topics in unsubcribe topic queue
 * 
 */
void unsubscribeFromTopics()
{
    if (unsubsTopicsQueue.count() > 0)
    {
        char unsubsStr[256];
        sprintf( unsubsStr, "Unsubscribing from: %s", unsubsTopicsQueue.peek().c_str());
        systemLog(tINFO, unsubsStr);
        
        if (_mqttClient->unsubscribe(unsubsTopicsQueue.pop().c_str()))
        {
            systemLog(tINFO, "Successfully unsubscribed!");
        }
        else
        {
            systemLog(tERROR, "There was a problem with unsubscribing!");
        }
    }
}

/**
 * @brief Subscribes to topics from subscribe topic pending queue
 * 
 */
void subscribeToTopics()
{
    if (subsTopicsPendingQueue.count() > 0)
    {
        char subsStr[256];
        sprintf( subsStr, "Subscribing from: %s", subsTopicsPendingQueue.peek().c_str());
        systemLog(tINFO, subsStr);

        if (_mqttClient->subscribe(subsTopicsPendingQueue.peek().c_str()))
        {
            subsTopicsSubscribedQueue.push(subsTopicsPendingQueue.pop()); // Add topics to Subscribed Queue, so we know to which topics we are subscribed
            systemLog(tINFO, "Successfully subscribed!");
        }
        else
        {
            systemLog(tERROR, "There was a problem subscribing!");
        }
    }
}

/**
 * @brief Transfers subscribed topics back to pending queue to resubscribe
 * 
 */
void restoreSubscribedTopics()
{
    uint16_t subsTopicsCount = subsTopicsSubscribedQueue.count();

    for (int i = 0; i < subsTopicsCount; i++)
    {
        subsTopicsPendingQueue.push(subsTopicsSubscribedQueue.pop());
    }
}


/**
 * @brief Adds topic to queue and subscribes as soon as possible
 * 
 * @param topic Topic name
 */
void mqttSubscribe(const char *topic)
{
    subsTopicsPendingQueue.push(String(topic));
    // If Queue is full data will be dropped
}

/**
 * @brief Unsubscribe from the MQTT topic - adds to unsubscribe queue
 * 
 * @param topic Topic name
 */
void mqttUnsubscribe(const char *topic)
{
    unsubsTopicsQueue.push(String(topic));
    // If Queue is full data will be dropped
}
