#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "spi_slave_task.h"
#include "spi.h"
#include <string.h>

#define SPI_BUF_SIZE 32

extern SPI_HandleTypeDef hspi2;

static uint8_t txBuf[SPI_BUF_SIZE] = "Reply from Slave!";
static uint8_t rxBuf[SPI_BUF_SIZE] = {0};

void SPISlaveTaskProc(void *argument)
{
    spi2DoneSem = xSemaphoreCreateBinary();
    if (spi2DoneSem == NULL) {
        printf("Failed to create SPI Slave semaphore\n");
        vTaskDelete(NULL);
    }

    for (;;) {
        memset(rxBuf, 0, sizeof(rxBuf));

        if (HAL_SPI_TransmitReceive_DMA(&hspi2, txBuf, rxBuf, SPI_BUF_SIZE) != HAL_OK) {
            printf("SPI Slave DMA start failed\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        printf("SPI Slave waiting\n");

        if (xSemaphoreTake(spi2DoneSem, pdMS_TO_TICKS(5000)) == pdTRUE) {
            printf("SPI Slave rcv[%s]\n", rxBuf);
        } else {
            printf("SPI DMA timeout\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
