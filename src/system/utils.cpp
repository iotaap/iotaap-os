#include "utils.h"
#include "definitions.h"
#include "time.h"
#include "./hmi/commands_engine.h"
#include "./fs/fs_module.h"
#include "./fs/local_data.h"
#include "./mqtt/mqtt_client.h"
#include "./hmi/led_task.h"
#include <HTTPClient.h>
#include "./system/system_tasks.h"
#include "./system/system_configuration.h"

/**
 * @brief Get the Battery Percentage
 * 
 * @return float 
 */
float getBatteryPercentage()
{
    float perc = round(((getVoltage(getAdcValue(BATSENS_PIN)) * 2) / 4.2) * 100);
    if (perc > 100)
        perc = 100;
    if (perc < 0)
        perc = 0;
    return perc;
}

/**
 * @brief Reads analog value from analog pin
 * 
 * @param pin Analog pin assigned
 * @return unsigned long 12bit analog value
 */
unsigned long getAdcValue(int pin)
{
    return analogRead(pin);
}

/**
 * @brief Returns voltage (V) from A/D reading
 * 
 * @param reading RAW A/D reading (0-4095);
 * @return float Calculated calibrated voltage
 */
float getVoltage(unsigned long reading)
{
    if (reading < 1 || reading > 4095)
        return 0;
    return (-0.000000000000016 * pow(reading, 4) + 0.000000000118171 * pow(reading, 3) - 0.000000301211691 * pow(reading, 2) + 0.001109019271794 * reading + 0.034143524634089);
}


/**
 * @brief Gets current system time in YYYY-MM-DD HH:MM:SS format
 * 
 */
char *getSystemTimeString( char *Timestamp)
{
    unsigned long TimeNow = millis();
    uint64_t epoch_ms = systemStat.epochTimeMs
                        + (TimeNow - systemStat.epochTimeUpdatedMs)
                        + SystemGetTimezoneOffsetMs();
    time_t epoch_s = epoch_ms/1000;

    strftime(Timestamp, TIME_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", localtime(&epoch_s));
    return Timestamp;
}

/**
 * @brief Gets current system time in YYYY-MM-DD HH:MM:SS format
 * 
 */
char *getSystemTimeString( char *Timestamp, time_t unix_s)
{
    unix_s += SystemGetTimezoneOffsetS();
    struct tm ts = *localtime(&unix_s);
    strftime(Timestamp, TIME_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", &ts);
    return Timestamp;
}

/**
 * @brief Get the Unix Time in ms
 * 
 * @return uint64_t unix time in ms
 */
uint64_t getUnixTimeMs()
{
    return systemStat.epochTimeMs + (millis() - systemStat.epochTimeUpdatedMs);
}


/**
 * @brief Get system uptime
 */
uint64_t getSystemUptimeS( void)
{
    unsigned long TimeNow = millis();
    uint64_t uptime_ms = systemStat.epochTimeMs
                        + (TimeNow - systemStat.epochTimeUpdatedMs)
                        - systemStat.bootTimeEpochMs;
    return uptime_ms/1000;
}

/**
 * @brief Initialize peripherals
 * 
 */
void periphInit()
{
    Serial.begin(115200);
    LedPinInit(LED1);
}

/**
 * @brief Create ssid from MAC number
 */
void createSSID( char *ssid)
{
    uint64_t chipid = ESP.getEfuseMac() & 0xFFFFFFFFFFFF;
    snprintf(ssid, 32, "IoTaaP-%012llX", chipid);
}


/**
 * @brief Backup files if needed and restart device
 */
void induceReset( void)
{
    if (!systemStat.fsInitialized)
    {
        SetRestartTime();
        ESP.restart();
    }
    
    stopPublishing();
    backupDataAndRestart();
}