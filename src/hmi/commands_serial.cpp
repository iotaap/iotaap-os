#include "commands_serial.h"

#include "string.h"
#include "stdint.h"
#include "./fs/sys_logs_data.h"
#include "commands_engine.h"
#include "system/definitions.h"

/**
 * @brief Task for detecting and parsing serial inputDebugString
 */
void SerialTask(void *parameter)
{
    char Input[MaxCommandLen] = {0};
    while(1)
    {
        PRINT_EXTRA_STACK_IN_TASK();
        /* If there is data */
        while (Serial.available())
        {
            PRINT_EXTRA_STACK_IN_TASK();
            const char Letter = Serial.read();

            if (Letter == '\r')
            {
                /* Ignore '\r' character */
            }
            else if (Letter != '\n')
            /* Append character to string */
            {
                strncat( Input, &Letter, 1);

                /* Overflow protection - shift buffer and remove first char */
                if (strlen(Input) >= MaxCommandLen-1)
                {
                    for (int i=0; i<MaxCommandLen-1; Input[i]=Input[i+1],i++);
                    Input[MaxCommandLen-1] = '\0';
                }
            }
            else
            {
                ProcessCommand( Input, CommandSourceSerial);
                memset(Input, '\0', MaxCommandLen);
            }
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}



/**
 * @brief Set response element in JSON
 */
void SerialCommandSetResponse( char *Response)
{
    Serial.println( Response);
}


/**
 * @brief Add data to array. If data exceed given memory, send data and create new JSON
 */
void SerialCommandAddData( char *Data)
{
    Serial.print( Data);
}


/**
 * @brief Send created JSON
 */
void SerialCommandSendResponse( void)
{
}


/**
 * @brief Send status
 */
void SerialCommandSendStatus( char *Status)
{
    Serial.println( Status);
}
