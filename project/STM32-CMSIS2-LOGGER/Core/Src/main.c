/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "logger.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* Definitions for task0 */
osThreadId_t task0Handle;
uint32_t task0Buffer[128];
osStaticThreadDef_t task0ControlBlock;
const osThreadAttr_t task0_attributes = {
	.name = "task0",
	.stack_mem = &task0Buffer[0],
	.stack_size = sizeof(task0Buffer),
	.cb_mem = &task0ControlBlock,
	.cb_size = sizeof(task0ControlBlock),
	.priority = (osPriority_t)osPriorityAboveNormal,
};
/* Definitions for task1 */
osThreadId_t task1Handle;
uint32_t task1Buffer[128];
osStaticThreadDef_t task1ControlBlock;
const osThreadAttr_t task1_attributes = {
	.name = "task1",
	.stack_mem = &task1Buffer[0],
	.stack_size = sizeof(task1Buffer),
	.cb_mem = &task1ControlBlock,
	.cb_size = sizeof(task1ControlBlock),
	.priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for task2 */
osThreadId_t task2Handle;
uint32_t task2Buffer[128];
osStaticThreadDef_t task2ControlBlock;
const osThreadAttr_t task2_attributes = {
	.name = "task2",
	.stack_mem = &task2Buffer[0],
	.stack_size = sizeof(task2Buffer),
	.cb_mem = &task2ControlBlock,
	.cb_size = sizeof(task2ControlBlock),
	.priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for loggerTask */
osThreadId_t loggerTaskHandle;
uint32_t loggerTaskBuffer[128];
osStaticThreadDef_t loggerTaskControlBlock;
const osThreadAttr_t loggerTask_attributes = {
	.name = "loggerTask",
	.stack_mem = &loggerTaskBuffer[0],
	.stack_size = sizeof(loggerTaskBuffer),
	.cb_mem = &loggerTaskControlBlock,
	.cb_size = sizeof(loggerTaskControlBlock),
	.priority = (osPriority_t)osPriorityLow,
};
/* Definitions for queueLogger */
osMessageQueueId_t queueLoggerHandle;
uint8_t queueLoggerBuffer[32 * sizeof(LogMessage)];
osStaticMessageQDef_t queueLoggerControlBlock;
const osMessageQueueAttr_t queueLogger_attributes = {
	.name = "queueLogger",
	.cb_mem = &queueLoggerControlBlock,
	.cb_size = sizeof(queueLoggerControlBlock),
	.mq_mem = &queueLoggerBuffer,
	.mq_size = sizeof(queueLoggerBuffer)};
/* Definitions for ledMutex */
osMutexId_t ledMutexHandle;
osStaticMutexDef_t ledMutexControlBlock;
const osMutexAttr_t ledMutex_attributes = {
	.name = "ledMutex",
	.cb_mem = &ledMutexControlBlock,
	.cb_size = sizeof(ledMutexControlBlock),
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
void startTask0(void *argument);
void startTask1(void *argument);
void startTask2(void *argument);
void startLoggerTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();
	/* Create the mutex(es) */
	/* creation of ledMutex */
	ledMutexHandle = osMutexNew(&ledMutex_attributes);

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the queue(s) */
	/* creation of queueLogger */
	queueLoggerHandle = osMessageQueueNew(32, sizeof(LogMessage), &queueLogger_attributes);

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of task0 */
	task0Handle = osThreadNew(startTask0, NULL, &task0_attributes);

	/* creation of task1 */
	task1Handle = osThreadNew(startTask1, NULL, &task1_attributes);

	/* creation of task2 */
	task2Handle = osThreadNew(startTask2, NULL, &task2_attributes);

	/* creation of loggerTask */
	loggerTaskHandle = osThreadNew(startLoggerTask, NULL, &loggerTask_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
	RCC_OscInitStruct.PLL.PLLN = 85;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : BUTTON_Pin */
	GPIO_InitStruct.Pin = BUTTON_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LED_GREEN_Pin */
	GPIO_InitStruct.Pin = LED_GREEN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GREEN_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_startTask0 */
/**
 * @brief  Function implementing the task0 thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_startTask0 */
void startTask0(void *argument)
{
	/* USER CODE BEGIN 5 */

	UNUSED(argument);
	osDelay(500);
	/* Infinite loop */
	for (;;)
	{
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
		osDelay(1000);
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_startTask1 */
/**
 * @brief Function implementing the task1 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startTask1 */
void startTask1(void *argument)
{
	/* USER CODE BEGIN startTask1 */

	UNUSED(argument);
	for (;;)
	{
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
		osDelay(1000);
	}
}
/* USER CODE END startTask1 */


/* USER CODE BEGIN Header_startTask2 */
/**
 * @brief Function implementing the task2 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startTask2 */
void startTask2(void *argument)
{
	/* USER CODE BEGIN startTask2 */
	UNUSED(argument);
	/* Infinite loop */
	for (;;)
	{
		osDelay(300);
		HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	}
	/* USER CODE END startTask2 */
}

/* USER CODE BEGIN Header_startLoggerTask */
/**
 * @brief Function implementing the loggerTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startLoggerTask */
void startLoggerTask(void *argument)
{
	/* USER CODE BEGIN startLoggerTask */
	UNUSED(argument);
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
	/* USER CODE END startLoggerTask */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6)
	{
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
