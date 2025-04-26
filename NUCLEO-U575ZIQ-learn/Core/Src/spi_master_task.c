#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "spi_master_task.h"
#include "spi.h"
#include <string.h>

#define SPI_BUF_SIZE 32

extern SPI_HandleTypeDef hspi1;

static uint8_t mTxBuf[SPI_BUF_SIZE] = "Hello from SPI DMA!";
static uint8_t mRxBuf[SPI_BUF_SIZE] = {0};

void SPIMasterTaskProc(void *argument)
{
    spi1TxDoneSem = xSemaphoreCreateBinary();
    spi1RxDoneSem = xSemaphoreCreateBinary();
    if (spi1TxDoneSem == NULL || spi1RxDoneSem == NULL) {
        uart_printf("Failed to create SPI Master semaphore\n");
        vTaskDelete(NULL);
    }

    for (;;) {
        memset(mRxBuf, 0, sizeof(mRxBuf));

        uart_printf("dmy");

        if (HAL_SPI_Transmit_DMA(&hspi1, mTxBuf, SPI_BUF_SIZE) != HAL_OK) {
            uart_printf("SPI Master DMA tx failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(spi1TxDoneSem, portMAX_DELAY) != pdTRUE) {
            uart_printf("SPI Master DMA tx timeout\n");
        }

        if (HAL_SPI_Receive_DMA(&hspi1, mRxBuf, SPI_BUF_SIZE) != HAL_OK) {
            uart_printf("SPI Master DMA rx failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(spi1RxDoneSem, portMAX_DELAY) == pdTRUE) {
            uart_printf("SPI Master DMA rcvd[%s]\n", mRxBuf);
        } else {
            uart_printf("SPI Master DMA rx timeout\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
