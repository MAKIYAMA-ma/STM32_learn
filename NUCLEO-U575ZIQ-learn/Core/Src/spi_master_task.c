#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "spi_master_task.h"
#include "spi.h"
#include <string.h>

#define SPI_BUF_SIZE 32

extern SPI_HandleTypeDef hspi1;

static uint8_t txBuf[SPI_BUF_SIZE] = "Hello from SPI DMA!";
static uint8_t rxBuf[SPI_BUF_SIZE] = {0};

void SPIMasterTaskProc(void *argument)
{
    spi1DoneSem = xSemaphoreCreateBinary();
    if (spi1DoneSem == NULL) {
        printf("Failed to create SPI Master semaphore\n");
        vTaskDelete(NULL);
    }

    for (;;) {
        memset(rxBuf, 0, sizeof(rxBuf));

        if (HAL_SPI_TransmitReceive_DMA(&hspi1, txBuf, rxBuf, SPI_BUF_SIZE) != HAL_OK) {
            printf("SPI Master DMA start failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        printf("SPI Master waiting\n");

        if (xSemaphoreTake(spi1DoneSem, pdMS_TO_TICKS(1000)) == pdTRUE) {
            printf("SPI Master rcv[%s]\n", rxBuf);
        } else {
            printf("SPI DMA timeout\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
