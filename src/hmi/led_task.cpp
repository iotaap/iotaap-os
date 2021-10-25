#include "./hmi/led_task.h"
#include <Arduino.h>

/*****************************************************************************/
/**
 * Led task states
 */
enum eLedTaskState
{
    LED_STATE_OFF,
    LED_STATE_ON,
	LED_STATE_BLINK,
};

/**
 * Led definition
 */
struct sLed
{
    unsigned int PinNum;
    unsigned int Timer;
    unsigned int LowPeriod;
    unsigned int HighPeriod;
    enum eLedTaskState TaskState;
};

/*****************************************************************************/

/**
 * Struct to store data necessary for task
 */
struct sLed Led;

/**
 * @brief   Task for LED control
 * @details Only one led is possible for now
 */
void LedTask(void *parameter)
{
    while(1)
    {
        /* If LED pin is not initialized */
        while(!Led.PinNum)
        {
            vTaskDelay(LED_TASK_PERIOD_MS / portTICK_PERIOD_MS);
        };

        switch(Led.TaskState)
        {
            // -----------------------------------------------------------------
            case LED_STATE_ON:
            {
                digitalWrite(Led.PinNum, HIGH);
                break;
            }

            // -----------------------------------------------------------------
            case LED_STATE_OFF:
            {
                digitalWrite(Led.PinNum, LOW);
                break;
            }

            // -----------------------------------------------------------------
            case LED_STATE_BLINK:
            {
                int LedStatus = digitalRead(Led.PinNum);
                Led.Timer += LED_TASK_PERIOD_MS;

                if (LedStatus && (Led.Timer > Led.HighPeriod))
                {
                    digitalWrite(Led.PinNum, LOW);
                    Led.Timer = 0;
                }
                else if (!LedStatus && (Led.Timer > Led.LowPeriod))
                {
                    digitalWrite(Led.PinNum, HIGH);
                    Led.Timer = 0;
                }
                break;
            }
            // -----------------------------------------------------------------
            default:
            {
                Led.TaskState = LED_STATE_OFF;
                break;
            }

        } // switch

        vTaskDelay(LED_TASK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

/**
 * @brief   Initialize pin for LED output
 */
void LedPinInit(unsigned int PinNum)
{
    pinMode(PinNum, OUTPUT);
    Led.PinNum = PinNum;
}

/**
 * @brief   Led control functions
 * @details LedSwitchOn()
 *          LedSwitchOff()
 *          LedBlinkFast() 80/80 ms
 *          LedBlinkConfigurator() 500/50 ms
 *          LedBlinkSlow() 300/300 ms
 *          LedBlinkPeriodic() 50/10000 ms
 *          LedBlinkUser(unsigned int, unsigned int) on/off ms
 */
void LedSwitchOn(void)
{
    Led.TaskState = LED_STATE_ON;
    Led.Timer = 0;
}
void LedSwitchOff(void)
{
    Led.TaskState = LED_STATE_OFF;
    Led.Timer = 0;
}
void LedBlinkFast(void)
{
    Led.TaskState = LED_STATE_BLINK;
    Led.HighPeriod = 100;
    Led.LowPeriod = 100;
}
void LedBlinkConfigurator(void)
{
    Led.TaskState = LED_STATE_BLINK;
    Led.HighPeriod = 500;
    Led.LowPeriod = 100;
}
void LedBlinkSlow(void)
{
    Led.TaskState = LED_STATE_BLINK;
    Led.HighPeriod = 300;
    Led.LowPeriod = 300;
}
void LedBlinkPeriodic(void)
{
    Led.TaskState = LED_STATE_BLINK;
    Led.HighPeriod = 100;
    Led.LowPeriod = 10000;
}
void LedBlinkUser(unsigned int HighPeriodMs, unsigned int LowPeriodMs)
{
    Led.TaskState = LED_STATE_BLINK;
    Led.HighPeriod = HighPeriodMs;
    Led.LowPeriod = LowPeriodMs;
}
