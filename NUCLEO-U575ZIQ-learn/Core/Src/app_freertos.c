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
#include "spi_master_task.h"
#include "spi_slave_task.h"
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
  .stack_size = 128 * 4
};
/* Definitions for SPIMasterTask */
osThreadId_t SPIMasterTaskHandle;
const osThreadAttr_t SPIMasterTask_attributes = {
  .name = "SPIMasterTask",
  .priority = (osPriority_t) osPriorityAboveNormal1,
  .stack_size = 128 * 4
};
/* Definitions for SPISlaveTask */
osThreadId_t SPISlaveTaskHandle;
const osThreadAttr_t SPISlaveTask_attributes = {
  .name = "SPISlaveTask",
  .priority = (osPriority_t) osPriorityAboveNormal1,
  .stack_size = 128 * 4
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

  /* creation of SPIMasterTask */
  SPIMasterTaskHandle = osThreadNew(StartSPIMasterTask, NULL, &SPIMasterTask_attributes);

  /* creation of SPISlaveTask */
  SPISlaveTaskHandle = osThreadNew(StartSPISlaveTask, NULL, &SPISlaveTask_attributes);

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
        uart_printf(DBG_LVL_DBG, "dummy message from StartSerialTask[%lu]\n", cycle_cnt);
        cycle_cnt = (cycle_cnt+1)%1000000;
        osDelay(10*1000);
    }
  /* USER CODE END SerialTask */
}

/* USER CODE BEGIN Header_StartSPIMasterTask */
/**
* @brief Function implementing the SPIMasterTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSPIMasterTask */
void StartSPIMasterTask(void *argument)
{
  /* USER CODE BEGIN SPIMasterTask */
    /* Infinite loop */
#if SPI_TEST_MODE == 0
    SPILoopTaskProc(argument);
#elif SPI_TEST_MODE == 1
    SPIMasterTaskProc(argument);
#else
    for(;;)
    {
        osDelay(1);
    }
#endif
  /* USER CODE END SPIMasterTask */
}

/* USER CODE BEGIN Header_StartSPISlaveTask */
/**
* @brief Function implementing the SPISlaveTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSPISlaveTask */
void StartSPISlaveTask(void *argument)
{
  /* USER CODE BEGIN SPISlaveTask */
    /* Infinite loop */
#if SPI_TEST_MODE == 1
    SPISlaveTaskProc(argument);
#else
    for(;;)
    {
        osDelay(1);
    }
#endif
  /* USER CODE END SPISlaveTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

