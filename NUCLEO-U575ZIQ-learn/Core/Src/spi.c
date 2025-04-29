#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "spi.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

SemaphoreHandle_t spiDoneSem;
SemaphoreHandle_t spi1TxDoneSem;
SemaphoreHandle_t spi1RxDoneSem;
SemaphoreHandle_t spi2TxDoneSem;
SemaphoreHandle_t spi2RxDoneSem;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(spi1TxDoneSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    if (hspi->Instance == SPI2) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(spi2TxDoneSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(spi1RxDoneSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    if (hspi->Instance == SPI2) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(spi2RxDoneSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(spiDoneSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        uart_printf(DBG_LVL_ERROR, "SPI Master Error: 0x%lx\n", hspi->ErrorCode);
    } else if (hspi->Instance == SPI2) {
        uart_printf(DBG_LVL_ERROR, "SPI Slave Error: 0x%lx\n", hspi->ErrorCode);
    }
}
