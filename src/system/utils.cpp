#include "utils.h"
#include "definitions.h"
#include "time.h"
#include "./hmi/commands_engine.h"
#include "./fs/fs_module.h"
#include "./hmi/led_task.h"
#include <HTTPClient.h>
#include "./system/system_tasks.h"

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
    strftime(Timestamp, TIME_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", &systemStat.systemTime);
    return Timestamp;
}

/**
 * @brief Get the System Time in ms
 * 
 * @return uint64_t system time in ms
 */
uint64_t getSystemTimeMs()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
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