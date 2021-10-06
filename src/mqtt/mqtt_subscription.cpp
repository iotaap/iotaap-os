#include "mqtt_subscription.h"

#include "./system/definitions.h"
#include "./fs/sys_logs_data.h"
#include "./mqtt/mqtt_client.h"
#include "./libs_3rd_party/LinkedList/LinkedList.h"

/* Max n topics can be added to queue. Once subscribed, topic will be added to subscribed queue */
static LinkedList<String> subsTopicsPendingQueue = LinkedList<String>();
static LinkedList<String> subsTopicsSubscribedQueue = LinkedList<String>();
static LinkedList<String> unsubsTopicsQueue = LinkedList<String>();


/**
 * @brief Unsubscribes from selected topics in unsubcribe topic queue
 * 
 */
void unsubscribeFromTopics()
{
    if (unsubsTopicsQueue.size() > 0)
    {
        char unsubsStr[256];
        sprintf( unsubsStr, "Unsubscribing from: %s", unsubsTopicsQueue.get(0).c_str());
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
    if (subsTopicsPendingQueue.size() > 0)
    {
        char subsStr[256];
        sprintf( subsStr, "Subscribing from: %s", subsTopicsPendingQueue.get(0).c_str());
        systemLog(tINFO, subsStr);

        if (_mqttClient->subscribe(subsTopicsPendingQueue.get(0).c_str()))
        {
            subsTopicsSubscribedQueue.add(subsTopicsPendingQueue.shift()); // Add topics to Subscribed Queue, so we know to which topics we are subscribed
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
    uint16_t subsTopicsCount = subsTopicsSubscribedQueue.size();

    for (int i = 0; i < subsTopicsCount; i++)
    {
        subsTopicsPendingQueue.add(subsTopicsSubscribedQueue.shift());
    }
}


/**
 * @brief Adds topic to queue and subscribes as soon as possible
 * 
 * @param topic Topic name
 */
void mqttSubscribe(const char *topic)
{
    subsTopicsPendingQueue.add(String(topic));
    // If Queue is full data will be dropped
}

/**
 * @brief Unsubscribe from the MQTT topic - adds to unsubscribe queue
 * 
 * @param topic Topic name
 */
void mqttUnsubscribe(const char *topic)
{
    String topicStr = String(topic);
    /* Check if already exist in subscribe queue - if so, drop it */
    for (int i=0; i<subsTopicsPendingQueue.size(); i++)
    {
        if (topicStr == subsTopicsPendingQueue.get(0))
        {
            subsTopicsPendingQueue.pop();
        }
        else
        {
            subsTopicsPendingQueue.add( subsTopicsPendingQueue.shift());
        }
    }
    /* Check if already exist in subscribe queue - if so, drop it */
    for (int i=0; i<subsTopicsSubscribedQueue.size(); i++)
    {
        if (topicStr == subsTopicsSubscribedQueue.get(0))
        {
            subsTopicsSubscribedQueue.pop();
        }
        else
        {
            subsTopicsSubscribedQueue.add( subsTopicsSubscribedQueue.shift());
        }
    }
    unsubsTopicsQueue.add(topicStr);
    // If Queue is full data will be dropped
}
