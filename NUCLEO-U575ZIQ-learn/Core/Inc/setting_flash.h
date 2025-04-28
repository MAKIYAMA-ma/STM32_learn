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

typedef struct {
    uint32_t version;
    uint8_t  node_name[16];
    uint8_t  dbg_lvl;
    uint8_t  red_led_int;
    uint32_t checksum;
} UserSetting_t;

extern UserSetting_t g_user_setting;

HAL_StatusTypeDef SaveUserSetting(UserSetting_t *setting);
HAL_StatusTypeDef LoadUserSetting(UserSetting_t *setting);
uint8_t InitUserSetting(void);

#ifdef __cplusplus
}
#endif

#endif /* __SETTING_FLASH_H */
/* USER CODE END */
