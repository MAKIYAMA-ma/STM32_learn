#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "spi_loop_task.h"
#include <string.h>

#define SPI_BUF_SIZE 32

extern SPI_HandleTypeDef hspi1;

static uint8_t txBuf[SPI_BUF_SIZE] = "Hello from SPI DMA!";
static uint8_t rxBuf[SPI_BUF_SIZE] = {0};

static SemaphoreHandle_t spiDoneSem;

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(spiDoneSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void SPILoopTaskProc(void *argument)
{
    spiDoneSem = xSemaphoreCreateBinary();
    if (spiDoneSem == NULL) {
        printf("Failed to create SPI semaphore\n");
        vTaskDelete(NULL);
    }

    for (;;) {
        memset(rxBuf, 0, sizeof(rxBuf));

        if (HAL_SPI_TransmitReceive_DMA(&hspi1, txBuf, rxBuf, SPI_BUF_SIZE) != HAL_OK) {
            printf("SPI DMA start failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (xSemaphoreTake(spiDoneSem, pdMS_TO_TICKS(1000)) == pdTRUE) {
            if (memcmp(txBuf, rxBuf, SPI_BUF_SIZE) == 0) {
                printf("SPI loopback OK: %s\n", rxBuf);
            } else {
                printf("SPI loopback failed!\n");
            }
        } else {
            printf("SPI DMA timeout\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
