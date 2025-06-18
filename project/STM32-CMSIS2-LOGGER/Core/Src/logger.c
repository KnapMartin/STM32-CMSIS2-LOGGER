#include "logger.h"

static osMessageQueueId_t *s_queueHandle;
bool s_init = false;

void __attribute__((weak)) LOG_uart_write(const char* buffer)
{
    UNUSED(buffer);
}

void LOG_init(osMessageQueueId_t *queueHandle)
{
    s_queueHandle = queueHandle;
    s_init = true;
}

void LOG_write(const char* format, ...)
{
    if (!s_init) return;

    LogMessage logMessage;
    va_list args;

    va_start(args, format);
    vsnprintf(logMessage.message, LOG_MSG_MAX_LEN, format, args);
    va_end(args);

    osMessageQueuePut(*s_queueHandle, &logMessage, 0, 0);
}

void LOG_task()
{
    LogMessage logMessage;
    while (true)
    {
        if (osMessageQueueGet(*s_queueHandle, &logMessage, NULL, osWaitForever) == osOK)
        {
            LOG_uart_write(logMessage.message);
        }
    }
}
