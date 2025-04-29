#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "spi_slave_task.h"
#include "spi.h"
#include <string.h>

#define SPI_BUF_SIZE 32

extern SPI_HandleTypeDef hspi2;

static uint8_t sTxBuf[SPI_BUF_SIZE] = "Reply from SPI Slave!";
static uint8_t sRxBuf[SPI_BUF_SIZE] = {0};

void SPISlaveTaskProc(void *argument)
{
    spi2TxDoneSem = xSemaphoreCreateBinary();
    spi2RxDoneSem = xSemaphoreCreateBinary();
    if (spi2TxDoneSem == NULL || spi2RxDoneSem == NULL) {
        uart_printf(DBG_LVL_ERROR, "Failed to create SPI Slave semaphore\n");
        vTaskDelete(NULL);
    }

    for (;;) {
        memset(sRxBuf, 0, sizeof(sRxBuf));

        if (HAL_SPI_Receive_DMA(&hspi2, sRxBuf, SPI_BUF_SIZE) != HAL_OK) {
            uart_printf(DBG_LVL_ERROR, "SPI Slave DMA rx failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(spi2RxDoneSem, portMAX_DELAY) == pdTRUE) {
            uart_printf(DBG_LVL_DBG, "SPI Slave DMA rcvd[%s]\n", sRxBuf);
        } else {
            uart_printf(DBG_LVL_DBG, "SPI Slave DMA rx timeout\n");
        }

        if (HAL_SPI_Transmit_DMA(&hspi2, sTxBuf, SPI_BUF_SIZE) != HAL_OK) {
            uart_printf(DBG_LVL_ERROR, "SPI Slave DMA tx failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(spi2TxDoneSem, portMAX_DELAY) != pdTRUE) {
            uart_printf(DBG_LVL_DBG, "SPI Slave DMA tx timeout\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
