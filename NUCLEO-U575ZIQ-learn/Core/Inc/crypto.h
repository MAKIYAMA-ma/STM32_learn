/* USER CODE BEGIN */
/**
  ******************************************************************************
  * @file           : crypto.h
  * @brief          : Header for spi_loop_task.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRYPTO_H
#define __CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_freertos.h"

int VerifySignature(uint8_t *firmware, size_t firmware_size, uint8_t *signature, size_t sig_len);
void TestVerifySignature(void);

#ifdef __cplusplus
}
#endif

#endif /* __CRYPTO_H */
/* USER CODE END */
