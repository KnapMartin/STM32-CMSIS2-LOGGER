#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#define LOG_MSG_MAX_LEN 128

typedef struct
{
    char message[LOG_MSG_MAX_LEN];
} LogMessage;


__attribute__((weak)) void LOG_uart_write(const char* buffer);
void LOG_init(osMessageQueueId_t *queueHandle);
void LOG_write(const char* format, ...);
void LOG_task();

#ifdef __cplusplus
}
#endif

#endif