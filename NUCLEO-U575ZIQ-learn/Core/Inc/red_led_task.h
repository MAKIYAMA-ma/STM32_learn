/* USER CODE BEGIN */
/**
  ******************************************************************************
  * @file           : setting_flash.h
  * @brief          : Header for setting_flash.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RED_LED_TASK_H
#define __RED_LED_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

void RedLEDTask(void);
void ButtonPressed(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* __RED_LED_TASK_H */
/* USER CODE END */
