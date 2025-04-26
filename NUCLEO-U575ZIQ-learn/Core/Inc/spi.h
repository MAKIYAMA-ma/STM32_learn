/* USER CODE BEGIN */
/**
  ******************************************************************************
  * @file           : spi_loop_task.h
  * @brief          : Header for spi_loop_task.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_freertos.h"

extern SemaphoreHandle_t spi1DoneSem;
extern SemaphoreHandle_t spi2DoneSem;

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H */
/* USER CODE END */
