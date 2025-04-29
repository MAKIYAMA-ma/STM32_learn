#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "spi_master_task.h"
#include "spi.h"
#include <string.h>

#define SPI_BUF_SIZE 32

extern SPI_HandleTypeDef hspi1;

static uint8_t mTxBuf[SPI_BUF_SIZE] = "Hello from SPI Master!";
static uint8_t mRxBuf[SPI_BUF_SIZE] = {0};

void SPIMasterTaskProc(void *argument)
{
    spi1TxDoneSem = xSemaphoreCreateBinary();
    spi1RxDoneSem = xSemaphoreCreateBinary();
    if (spi1TxDoneSem == NULL || spi1RxDoneSem == NULL) {
        uart_printf(DBG_LVL_ERROR, "Failed to create SPI Master semaphore\n");
        vTaskDelete(NULL);
    }

    for (;;) {
        memset(mRxBuf, 0, sizeof(mRxBuf));

        /* HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); */
        if (HAL_SPI_Transmit_DMA(&hspi1, mTxBuf, SPI_BUF_SIZE) != HAL_OK) {
            uart_printf(DBG_LVL_ERROR, "SPI Master DMA tx failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(spi1TxDoneSem, portMAX_DELAY) != pdTRUE) {
            uart_printf(DBG_LVL_DBG, "SPI Master DMA tx timeout\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1));   // Slaveの準備完了待ち 本来は待ち合わせ？

        if (HAL_SPI_Receive_DMA(&hspi1, mRxBuf, SPI_BUF_SIZE) != HAL_OK) {
            uart_printf(DBG_LVL_ERROR, "SPI Master DMA rx failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(spi1RxDoneSem, portMAX_DELAY) == pdTRUE) {
            uart_printf(DBG_LVL_DBG, "SPI Master DMA rcvd[%s]\n", mRxBuf);
        } else {
            uart_printf(DBG_LVL_DBG, "SPI Master DMA rx timeout\n");
        }
        /* HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); */

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
