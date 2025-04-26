/* USER CODE BEGIN */
/**
  ******************************************************************************
  * @file           : spi_loop_task.h
  * @brief          : Header for spi_loop_task.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_SLAVE_TASK_H
#define __SPI_SLAVE_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_freertos.h"

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void SPISlaveTaskProc(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_SLAVE_TASK_H */
/* USER CODE END */
