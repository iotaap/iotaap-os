#include <IoTaaP_OS.h>
#define TOKEN "<iotaap-link-secret>" // IoTaaP Link Secret

IoTaaP_OS iotaapOs("1.0.1");

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.println("---------------------------");
  Serial.println("Received data on topic:");
  Serial.println(topic); // Print topic

  Serial.println("Data:");

  for (int i = 0; i < length; i++) // Print message
  {
    Serial.print((char)message[i]);
  }
  Serial.println();
  Serial.println("---------------------------");
}

void setup()
{
  char deviceId[30];     // Char array used to store system parameter

  iotaapOs.start(); // Start IoTaaP OS

  iotaapOs.startWifi(); // Connect to WiFi
  iotaapOs.startMqtt(callback); // Connect to MQTT broker

  iotaapOs.writeToSystemLogs("Device started"); // Write data to system log using 'USER' tag

  iotaapOs.getSystemParameter("device_id", deviceId); // Get 'device_id' parameter from 'default.cfg'
  Serial.println("device_id parameter:");
  Serial.println(deviceId);

  iotaapOs.deviceCloudPublish("{\"my_text\":\"Hello IoT World!\"}", "hello_topic");        // Publish simple (escaped) JSON to: /<username>/devices/<device-id>/hello_topic
  iotaapOs.basicCloudPublish("{\"my_simple_text\":\"Hello IoT World!\"}", "simple_topic"); // Publish simple (escaped) JSON to: /<username>/simple_topic

  iotaapOs.basicSubscribe("dummy_topic"); // Subscribe to /<username>/dummy_topic

  iotaapOs.basicSubscribe("receiving_topic"); // Subscribe to /<username>/receiving_topic
  // Every message received on this topic will trigger callback

  iotaapOs.basicUnsubscribe("dummy_topic"); // Unsubscribe from /<username>/dummy_topic
  iotaapOs.customPublish("{\"my_text\":\"Hello IoT World!\"}", "hello_topic");
  iotaapOs.customSubscribe("dummy_topic");
  iotaapOs.customUnsubscribe("dummy_topic");

  iotaapOs.checkForUpdates(); // Manually check for updates at startup

  delay(1000);
}

void loop()
{
  iotaapOs.deviceCloudPublishParam("temp", random(0, 500) / 10.0); // Publish parameter (to topic: /<username>/devices/<device-id>/params)
  iotaapOs.deviceCloudPublishParam("humi", random(0, 1000) / 10.0); // Publish parameter (to topic: /<username>/devices/<device-id>/params)
  iotaapOs.deviceCloudPublishParam("pres", random(0, 15000) / 10.0); // Publish parameter (to topic: /<username>/devices/<device-id>/params)
  
    // Store temperature reading to IoTaaP Storage service, and subscribe to callback topic
  iotaapOs.storageServiceStore(TOKEN, "temperature", random(0, 500) / 10.0, "/pgiIzx7n/storage/response");

  // If temperature is higher than 30°C, send SMS using IoTaaP SMS service, and subscribe to callback topic
  if (random(0, 45) > 30)
  {
    char message[50];
    sprintf(message, "Current temperature is: %.1f°C", random(0, 500) / 10.0);
    iotaapOs.smsServiceSend(TOKEN, "+38599", message, "/pgiIzx7n/smsservice/response");
    iotaapOs.restart();
  }
  
  delay(1000);
}