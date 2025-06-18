#include "logger.h"

static osMessageQueueId_t *s_queueHandle;
static uint8_t s_init = 0;
static char s_formatted[LOG_MSG_MAX_LEN + 32];
static volatile uint8_t s_running = 1;

static void LOG_generic(LogType type, const char *format, va_list args);

__attribute__((weak)) void LOG_uart_write(const char *buffer)
{
    printf("%s", buffer);
}

void LOG_init(osMessageQueueId_t *queueHandle)
{
    s_queueHandle = queueHandle;
    s_init = 1;
}

static void LOG_generic(LogType type, const char *format, va_list args)
{
    if (!s_init)
        return;

    LogMessage logMessage;
#if LOG_TIMESTAMP
    logMessage.timestamp_ms = osKernelGetTickCount();
#endif
    vsnprintf(logMessage.message, LOG_MSG_MAX_LEN, format, args);
    logMessage.type = type;

    osStatus_t status = osMessageQueuePut(*s_queueHandle, &logMessage, 0, 0);
    if (status == osErrorResource) // Queue full
    {
        // Drop the oldest message
        LogMessage dummy;
        osMessageQueueGet(*s_queueHandle, &dummy, NULL, 0);
        (void)dummy; // Suppress unused variable warning
        // Try again to put the new message
        osMessageQueuePut(*s_queueHandle, &logMessage, 0, 0);
        // TODO: maybe print a warning here
    }
}

void LOG_INFO(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    LOG_generic(LOG_TYPE_INFO, format, args);
    va_end(args);
}

void LOG_WRN(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    LOG_generic(LOG_TYPE_WARNING, format, args);
    va_end(args);
}

void LOG_ERR(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    LOG_generic(LOG_TYPE_ERROR, format, args);
    va_end(args);
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
            snprintf(s_formatted, sizeof(s_formatted), "[%lu] [%s] %s\r\n", logMessage.timestamp_ms,
                     (logMessage.type == LOG_TYPE_INFO) ? "INFO" :
                     (logMessage.type == LOG_TYPE_WARNING) ? "\033[93mWARNING\033[0m" :
                     (logMessage.type == LOG_TYPE_ERROR) ? "\033[31mERROR\033[0m" : "",
                     logMessage.message);
#else
            snprintf(s_formatted, sizeof(s_formatted), "[%s] %s\r\n",
                     (logMessage.type == LOG_TYPE_INFO) ? "INFO" :
                     (logMessage.type == LOG_TYPE_WARNING) ? "\033[93mWARNING\033[0m" :
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
