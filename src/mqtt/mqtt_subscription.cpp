#include "mqtt_subscription.h"

#include "./system/definitions.h"
#include "./fs/sys_logs_data.h"
#include "./mqtt/mqtt_client.h"
#include <list>
using namespace std;

/* Max n topics can be added to queue. Once subscribed, topic will be added to subscribed queue */
static list <char*> subsTopicsPendingQueue;
static list <char*> subsTopicsSubscribedQueue;
static list <char*> unsubsTopicsQueue;


/**
 * @brief Unsubscribes from selected topics in unsubcribe topic queue
 * 
 */
void unsubscribeFromTopics()
{
    if (unsubsTopicsQueue.size() > 0)
    {
        char unsubsStr[256];
        sprintf( unsubsStr, "Unsubscribing from: %s", unsubsTopicsQueue.front());
        systemLog(tINFO, unsubsStr);
        
        if (_mqttClient->unsubscribe(unsubsTopicsQueue.front()))
        {
            unsubsTopicsQueue.pop_front();
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
        sprintf( subsStr, "Subscribing from: %s", subsTopicsPendingQueue.front());
        systemLog(tINFO, subsStr);

        if (_mqttClient->subscribe(subsTopicsPendingQueue.front()))
        {
            subsTopicsSubscribedQueue.push_back(subsTopicsPendingQueue.front()); // Add topics to Subscribed Queue, so we know to which topics we are subscribed
            subsTopicsPendingQueue.pop_front();
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
    while (subsTopicsSubscribedQueue.size())
    {
        subsTopicsPendingQueue.push_back(subsTopicsSubscribedQueue.front());
        subsTopicsSubscribedQueue.pop_front();
    }
}


/**
 * @brief Adds topic to queue and subscribes as soon as possible
 * 
 * @param topic Topic name
 */
void mqttSubscribe(const char *topic)
{
    char *data = new char[strlen(topic)+1];
    strcpy( data, topic);
    subsTopicsPendingQueue.push_back( data);
}

/**
 * @brief Unsubscribe from the MQTT topic - adds to unsubscribe queue
 * 
 * @param topic Topic name
 */
void mqttUnsubscribe(const char *topic)
{
    char *data = new char[strlen(topic)+1];
    strcpy( data, topic);

    /* Check if already exist in subscribe queue - if so, drop it */
    for (int i=0; i<subsTopicsPendingQueue.size(); i++)
    {
        if (strcmp( data, subsTopicsPendingQueue.front()))
        {
            subsTopicsPendingQueue.push_back( subsTopicsPendingQueue.front());
        }
        subsTopicsPendingQueue.pop_front();
    }
    /* Check if already exist in subscribe queue - if so, drop it */
    for (int i=0; i<subsTopicsSubscribedQueue.size(); i++)
    {
        if (strcmp( data, subsTopicsSubscribedQueue.front()))
        {
            subsTopicsSubscribedQueue.push_back( subsTopicsSubscribedQueue.front());
        }
        subsTopicsSubscribedQueue.pop_front();
    }
    unsubsTopicsQueue.push_back( data);
}
