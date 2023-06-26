#include <IoTaaP_OS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Adafruit_BME280.h"

#define TOKEN "<iotaap-link-secret>" // IoTaaP Link Secret

Adafruit_BME280 bme;         // I2C
IoTaaP_OS iotaapOs("3.2.8"); // Defining Firmware version

// IoTaaP Network (MQTT) callback function
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.println("-------#-----#-----#----------");
  Serial.println("Received data on the topic:");
  Serial.println(topic); // Print topic

  Serial.println("Data:");

  for (int i = 0; i < length; i++) // Print message
  {
    Serial.print((char)message[i]);
  }
  Serial.println();
  Serial.println("------#------#------#---------");
}

void setup()
{
  // BME280 related stuff
  unsigned status;
  status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    while (1)
      delay(10);
  }

  char device_token[30]; // Char array used to store system parameter

  iotaapOs.start(); // Start IoTaaP OS

  iotaapOs.startWifi();         // Connect to WiFi
  iotaapOs.startMqtt(callback); // Connect to MQTT broker

  iotaapOs.writeToSystemLogs("Device started"); // Write data to system log using 'USER' tag

  iotaapOs.getSystemParameter("device_token", device_token); // Get 'device_id' parameter from 'default.cfg'
  Serial.println("device_token parameter:");
  Serial.println(device_token); // Prints device_token parameter

  iotaapOs.basicSubscribe("informer"); // Subscribe to /<username>/informer topic
}

DynamicJsonDocument doc(1024);
char payloadStr[512];

void loop()
{

  // Fetch measurements from BME280 sensor
  float temperatureC = bme.readTemperature();
  float humidityPerc = bme.readHumidity();
  float pressureHpa = bme.readPressure() / 100;

  // Create JSON document
  doc["temperatureC"] = temperatureC;
  doc["humidityPerc"] = humidityPerc;
  doc["pressureHpa"] = pressureHpa;

  serializeJson(doc, payloadStr);
  iotaapOs.deviceCloudPublish(payloadStr, "weather"); // Publish to /<username>/devices/<device-id>/weather

  // Publish parameter to /<username>/devices/<device-id>/params
  iotaapOs.deviceCloudPublishParam("temperatureC", temperatureC);
  iotaapOs.deviceCloudPublishParam("humidityPerc", humidityPerc);
  iotaapOs.deviceCloudPublishParam("pressureHpa", pressureHpa);

  // Store temperature reading to IoTaaP Storage service, and subscribe to callback topic
  iotaapOs.storageServiceStore(TOKEN, "temperature", temperatureC, "/pgiIzx7n/storage/response");

  // If temperature is higher than 30°C, send SMS using IoTaaP SMS service, and subscribe to callback topic
  if (temperatureC > 30)
  {
    char message[50];
    sprintf(message, "Current temperature is: %.1f°C", temperatureC);
    iotaapOs.smsServiceSend(TOKEN, "+385994158386", message, "/pgiIzx7n/smsservice/response");
  }

  delay(5000);
}
