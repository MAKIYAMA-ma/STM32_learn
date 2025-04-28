/* USER CODE BEGIN */
/**
  ******************************************************************************
  * @file           : debug_uart.h
  * @brief          : Header for debug_uart.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEBUG_UART_H
#define __DEBUG_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_freertos.h"

typedef enum {
    DBG_LVL_NONE = 0,
    DBG_LVL_ERROR,
    DBG_LVL_WARN,
    DBG_LVL_DBG,
    DBG_LVL_INFO,

    DBG_LVL_MAX = DBG_LVL_INFO,
} EN_DBG_LEVEL;

typedef enum {
    DBG_CMD_DBGLVL,

    DBG_CMD_NUM,
} EN_DBG_CMD;

void uart_init();
void uart_printf(EN_DBG_LEVEL lvl, const char *fmt, ...);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_UART_H */
/* USER CODE END */
