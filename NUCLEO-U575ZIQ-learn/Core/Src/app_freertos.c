/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "red_led_task.h"
#include "spi_loop_task.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for RedLEDTask */
osThreadId_t RedLEDTaskHandle;
const osThreadAttr_t RedLEDTask_attributes = {
  .name = "RedLEDTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for BlueLEDTask */
osThreadId_t BlueLEDTaskHandle;
const osThreadAttr_t BlueLEDTask_attributes = {
  .name = "BlueLEDTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for SerialTask */
osThreadId_t SerialTaskHandle;
const osThreadAttr_t SerialTask_attributes = {
  .name = "SerialTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 512 * 4
};
/* Definitions for SPILoopTask */
osThreadId_t SPILoopTaskHandle;
const osThreadAttr_t SPILoopTask_attributes = {
  .name = "SPILoopTask",
  .priority = (osPriority_t) osPriorityAboveNormal1,
  .stack_size = 512 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of RedLEDTask */
  RedLEDTaskHandle = osThreadNew(StartRedLEDTask, NULL, &RedLEDTask_attributes);

  /* creation of BlueLEDTask */
  BlueLEDTaskHandle = osThreadNew(StartBlueLEDTask, NULL, &BlueLEDTask_attributes);

  /* creation of SerialTask */
  SerialTaskHandle = osThreadNew(StartSerialTask, NULL, &SerialTask_attributes);

  /* creation of SPILoopTask */
  SPILoopTaskHandle = osThreadNew(StartSPILoopTask, NULL, &SPILoopTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_StartRedLEDTask */
/**
* @brief Function implementing the RedLEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRedLEDTask */
void StartRedLEDTask(void *argument)
{
  /* USER CODE BEGIN RedLEDTask */
    /* Infinite loop */
    for(;;)
    {
        RedLEDTask();
    }
  /* USER CODE END RedLEDTask */
}

/* USER CODE BEGIN Header_StartBlueLEDTask */
/**
* @brief Function implementing the BlueLEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBlueLEDTask */
void StartBlueLEDTask(void *argument)
{
  /* USER CODE BEGIN BlueLEDTask */
    /* Infinite loop */
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
        osDelay(1000);
    }
  /* USER CODE END BlueLEDTask */
}

/* USER CODE BEGIN Header_StartSerialTask */
/**
* @brief Function implementing the SerialTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSerialTask */
void StartSerialTask(void *argument)
{
  /* USER CODE BEGIN SerialTask */
    uint32_t cycle_cnt = 0;

    /* Infinite loop */
    for(;;)
    {
        printf("dummy message from StartSerialTask[%lu]\n", cycle_cnt);
        cycle_cnt = (cycle_cnt+1)%1000000;
        osDelay(10*1000);
    }
  /* USER CODE END SerialTask */
}

/* USER CODE BEGIN Header_StartSPILoopTask */
/**
* @brief Function implementing the SPILoopTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSPILoopTask */
void StartSPILoopTask(void *argument)
{
  /* USER CODE BEGIN SPILoopTask */
    /* Infinite loop */
    SPILoopTaskProc(argument);
  /* USER CODE END SPILoopTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

