#include "logger.h"


static osMessageQueueId_t *s_queueHandle;
uint8_t s_init = 0;

__attribute__((weak)) void LOG_uart_write(const char* buffer)
{
    printf(buffer);
}

void LOG_init(osMessageQueueId_t *queueHandle)
{
    s_queueHandle = queueHandle;
    s_init = 1;
}

void LOG_write(const char* format, ...)
{
    if (!s_init) return;

    LogMessage logMessage;
    va_list args;

    va_start(args, format);
    vsnprintf(logMessage.message, LOG_MSG_MAX_LEN, format, args);
    va_end(args);

    osMessageQueuePut(*s_queueHandle, &logMessage, 0, 0); // TODO: error handling
}

void LOG_task()
{
    if (!s_init) return;

    LogMessage logMessage;
    while (1)
    {
        if (osMessageQueueGet(*s_queueHandle, &logMessage, NULL, osWaitForever) == osOK)
        {
            LOG_uart_write(logMessage.message);
        }
    }
}
