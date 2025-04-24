/* USER CODE BEGIN */
/**
  ******************************************************************************
  * @file           : setting_flash.h
  * @brief          : Header for setting_flash.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SETTING_FLASH_H
#define __SETTING_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>

HAL_StatusTypeDef Settings_Write(const uint8_t *data, size_t len);
void Settings_Read(uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SETTING_FLASH_H */
/* USER CODE END */
