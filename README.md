# STM32-CMSIS2-LOGGER

An asynchronous, thread-safe logger module for STM32 projects using CMSIS-RTOS2.  
Designed for efficient, non-blocking log message handling with a dedicated logger task and message queue.

---

## Features

- **Asynchronous logging:** Offloads log formatting to a dedicated task.
- **Thread-safe:** Log from any task without blocking or data corruption.
- **Backpressure handling:** Drops oldest messages if the queue is full.
- **Configurable:** Easily adjust queue size, stack size, and priorities.
- **Simple API:** Use `LOG_INFO`, `LOG_WRN`, or `LOG_ERR` from any task.

---

## Usage

1. **Define the logger task and queue**  
   (For static allocation, ensure buffers are sized appropriately):

    ```c
    osThreadId_t loggerTaskHandle;
    uint32_t loggerTaskBuffer[256];
    osStaticThreadDef_t loggerTaskControlBlock;
    const osThreadAttr_t loggerTask_attributes = {
        .name = "loggerTask",
        .stack_mem = &loggerTaskBuffer[0],
        .stack_size = sizeof(loggerTaskBuffer),
        .cb_mem = &loggerTaskControlBlock,
        .cb_size = sizeof(loggerTaskControlBlock),
        .priority = (osPriority_t)osPriorityLow,
    };

    osMessageQueueId_t loggerQueueHandle;
    uint8_t loggerQueueBuffer[32 * sizeof(LogMessage)];
    osStaticMessageQDef_t loggerQueueControlBlock;
    const osMessageQueueAttr_t loggerQueue_attributes = {
        .name = "loggerQueue",
        .cb_mem = &loggerQueueControlBlock,
        .cb_size = sizeof(loggerQueueControlBlock),
        .mq_mem = &loggerQueueBuffer,
        .mq_size = sizeof(loggerQueueBuffer)
    };
    ```

2. **Initialize the logger module before starting the kernel:**

    ```c
    LOG_init(&loggerQueueHandle);
    ```

3. **Create the logger queue and task:**

    ```c
    loggerQueueHandle = osMessageQueueNew(32, sizeof(LogMessage), &loggerQueue_attributes);
    loggerTaskHandle = osThreadNew(startLoggerTask, NULL, &loggerTask_attributes);
    ```

4. **Implement the logger task:**

    ```c
    void startLoggerTask(void *argument)
    {
        UNUSED(argument);
        for (;;)
        {
            LOG_task();
        }
    }
    ```

5. **Log from any task:**

    ```c
    LOG_INFO("System started: %d", system_status);
    LOG_WRN("Low battery: %u%%", battery_level);
    LOG_ERR("Sensor failure: %s", sensor_name);
    ```
 6. **(OPTIONAL) Define yout `LOG_write` function:**

    ```c
      void LOG_write(const char *buffer)
      {
          my_write();
      }
    ```

---

## Notes

- Ensure the logger task has enough stack for message formatting.
- The logger queue size determines how many messages can be buffered.

---

## License

MIT License
