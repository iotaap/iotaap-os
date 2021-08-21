/*
 * IoTaaP OS
 * Open Source IoT Operating System
 * 
 * IoTaaP Team <contact@iotaap.io>
 * https://www.iotaap.io
 * 
 */

#ifndef __IOTAAP_OS_CORE_H__
#define __IOTAAP_OS_CORE_H__

#include "./system/definitions.h"
#include <Arduino.h>
#include "./libs_3rd_party/pubsubclient-2.8/src/PubSubClient.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <time.h>

class IoTaaP_OS
{
public:
    IoTaaP_OS(const char *fwVersion);
    void start();
    void startWifi();
    void startMqtt(MQTT_CALLBACK_SIGNATURE);
    int deviceCloudPublish(const char *payload, const char *uTopic);
    int basicCloudPublish(const char *payload, const char *uTopic);
    int basicSubscribe(const char *uTopic);
    int basicUnsubscribe(const char *uTopic);
    int customSubscribe(const char *uTopic);
    int customUnsubscribe(const char *uTopic);
    int deviceCloudPublishParam(const char *name, float value);
    int deviceCloudPublishParam(const char *name, const char *value);
    int deviceCloudPublishParamBatch(const char *name, float value);
    int deviceCloudPublishParamBatch(const char *name, const char *value);
    void writeToSystemLogs(const char *data);
    /* User parameter functions are not implemented since writing user parameters to internal FAT is not implemented */
    // bool getUserParameter(const char *element, char *output);
    // bool getUserParameter(const char *element, int *output);
    // bool getUserParameter(const char *element, bool *output);
    bool getSystemParameter(const char *element, char *output);
    bool getSystemParameter(const char *element, int *output);
    bool getSystemParameter(const char *element, bool *output);
    void checkForUpdates();
};

#endif