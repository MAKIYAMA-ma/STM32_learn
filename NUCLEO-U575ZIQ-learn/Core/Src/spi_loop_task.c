#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "spi_loop_task.h"
#include "spi.h"
#include <string.h>

#define SPI_BUF_SIZE 32

extern SPI_HandleTypeDef hspi1;

static uint8_t txBuf[SPI_BUF_SIZE] = "Test Message of SPI loopback";
static uint8_t rxBuf[SPI_BUF_SIZE] = {0};

void SPILoopTaskProc(void *argument)
{
    spiDoneSem = xSemaphoreCreateBinary();
    if (spiDoneSem == NULL) {
        uart_printf(DBG_LVL_ERROR, "Failed to create SPI semaphore\n");
        vTaskDelete(NULL);
    }

    for (;;) {
        memset(rxBuf, 0, sizeof(rxBuf));

#if 1   // use DMA
        if (HAL_SPI_TransmitReceive_DMA(&hspi1, txBuf, rxBuf, SPI_BUF_SIZE) != HAL_OK) {
            uart_printf(DBG_LVL_ERROR, "SPI DMA start failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(spiDoneSem, pdMS_TO_TICKS(1000)) == pdTRUE) {
            if (memcmp(txBuf, rxBuf, SPI_BUF_SIZE) == 0) {
                uart_printf(DBG_LVL_DBG, "SPI loopback OK: %s\n", rxBuf);
            } else {
                uart_printf(DBG_LVL_DBG, "SPI loopback failed!\n");
            }
        } else {
            uart_printf(DBG_LVL_DBG, "SPI DMA timeout\n");
        }
#else
        if (HAL_SPI_TransmitReceive(&hspi1, txBuf, rxBuf, SPI_BUF_SIZE, HAL_MAX_DELAY) == HAL_OK) {
            if (memcmp(txBuf, rxBuf, SPI_BUF_SIZE) == 0) {
                uart_printf(DBG_LVL_DBG, "SPI loopback OK: %s\n", rxBuf);
            } else {
                uart_printf(DBG_LVL_DBG, "SPI loopback failed!\n");
            }
        } else {
            uart_printf(DBG_LVL_DBG, "SPI Master failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
#endif

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
