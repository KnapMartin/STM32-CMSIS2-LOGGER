#include "logger.h"

static osMessageQueueId_t *s_queueHandle;
static uint8_t s_init = 0;
static char s_formatted[LOG_MSG_MAX_LEN + 32];
static volatile uint8_t s_running = 1;

__attribute__((weak)) void LOG_uart_write(const char *buffer)
{
    printf("%s", buffer);
}

void LOG_init(osMessageQueueId_t *queueHandle)
{
    s_queueHandle = queueHandle;
    s_init = 1;
}

void LOG_INFO(const char *format, ...)
{
    if (!s_init)
        return;

    LogMessage logMessage;
    va_list args;
#if LOG_TIMESTAMP
    logMessage.timestamp_ms = osKernelGetTickCount();
#endif
    va_start(args, format);
    vsnprintf(logMessage.message, LOG_MSG_MAX_LEN, format, args);
    va_end(args);

    logMessage.type = LOG_TYPE_INFO;

    osStatus_t status = osMessageQueuePut(*s_queueHandle, &logMessage, 0, 0);
    if (status != osOK)
    {
        // Handle error if needed, e.g., log to a different output or ignore
        // For now, we do nothing
    }
}

void LOG_WRN(const char *format, ...)
{
    if (!s_init)
        return;

    LogMessage logMessage;
    va_list args;
#if LOG_TIMESTAMP
    logMessage.timestamp_ms = osKernelGetTickCount();
#endif
    va_start(args, format);
    vsnprintf(logMessage.message, LOG_MSG_MAX_LEN, format, args);
    va_end(args);

    logMessage.type = LOG_TYPE_WARNING;

    osStatus_t status = osMessageQueuePut(*s_queueHandle, &logMessage, 0, 0);
    if (status != osOK)
    {
        ;
    }
}

void LOG_ERR(const char *format, ...)
{
    if (!s_init)
        return;

    LogMessage logMessage;
    va_list args;
#if LOG_TIMESTAMP
    logMessage.timestamp_ms = osKernelGetTickCount();
#endif
    va_start(args, format);
    vsnprintf(logMessage.message, LOG_MSG_MAX_LEN, format, args);
    va_end(args);

    logMessage.type = LOG_TYPE_ERROR;

    osStatus_t status = osMessageQueuePut(*s_queueHandle, &logMessage, 0, 0);
    if (status != osOK)
    {
        ;
    }
}

void LOG_task()
{
    if (!s_init)
        return;

    LogMessage logMessage;
    while (s_running)
    {
        osStatus_t status = osMessageQueueGet(*s_queueHandle, &logMessage, NULL, 10); // 10 ms timeout
        if (status == osOK)
        {
#if LOG_TIMESTAMP
            // snprintf(s_formatted, sizeof(s_formatted), "[%lu ms] %s\r\n", logMessage.timestamp_ms, logMessage.message);

            snprintf(s_formatted, sizeof(s_formatted), "[%lu] [%s] %s\r\n", logMessage.timestamp_ms,
                     (logMessage.type == LOG_TYPE_INFO) ? "INFO" :
                     (logMessage.type == LOG_TYPE_WARNING) ? "\033[38;5;208mWARNING\033[0m" :
                     (logMessage.type == LOG_TYPE_ERROR) ? "\033[31mERROR\033[0m" : "",
                     logMessage.message);
#else
            snprintf(s_formatted, sizeof(s_formatted), "[%s] %s\r\n",
                     (logMessage.type == LOG_TYPE_INFO) ? "INFO" :
                     (logMessage.type == LOG_TYPE_WARNING) ? "\033[38;5;208mWARNING\033[0m" :
                     (logMessage.type == LOG_TYPE_ERROR) ? "\033[31mERROR\033[0m" : "",
                     logMessage.message);
#endif
            s_formatted[sizeof(s_formatted) - 1] = '\0'; // Ensure null termination
            // Write to UART or other output
            LOG_uart_write(s_formatted);
        }
        else if (status == osErrorTimeout)
        {
            osDelay(1); // Short delay to yield CPU
        }
    }
}
