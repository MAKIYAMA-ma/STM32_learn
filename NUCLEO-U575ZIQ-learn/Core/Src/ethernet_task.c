#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
/* #include "ethernet_task.h" */
#include "spi.h"
#include <string.h>
#include "socket.h"
#include "wizchip_conf.h"

#define SPI_BUF_SIZE 32

extern SPI_HandleTypeDef hspi1;

void W5500_Select(void)
{
    HAL_GPIO_WritePin(W5500_CS_Port, W5500_CS_Pin, GPIO_PIN_RESET);
}

void W5500_Unselect(void)
{
    HAL_GPIO_WritePin(W5500_CS_Port, W5500_CS_Pin, GPIO_PIN_SET);
}

void W5500_ReadBuff(uint8_t* buff, uint16_t len)
{
    if(HAL_SPI_Receive_DMA(&hspi1, buff, len) != HAL_OK) {
        // TODO エラー処理
        return;
    }
    if (xSemaphoreTake(spi1TxDoneSem, portMAX_DELAY) != pdTRUE) {
        uart_printf(DBG_LVL_DBG, "SPI Master DMA tx timeout\n");
    }
}

void W5500_WriteBuff(uint8_t* buff, uint16_t len)
{
    if(HAL_SPI_Transmit_DMA(&hspi1, buff, len) != HAL_OK) {
        // TODO エラー処理
        return;
    }
    if (xSemaphoreTake(spi1RxDoneSem, portMAX_DELAY) != pdTRUE) {
        uart_printf(DBG_LVL_DBG, "SPI Master DMA rx timeout\n");
    }
}

uint8_t W5500_ReadByte(void)
{
    uint8_t data;
    W5500_ReadBuff(&data, sizeof(data));
    return data;
}

void W5500_WriteByte(uint8_t data)
{
    W5500_WriteBuff(&data, sizeof(data));
}

void W5500_Init()
{
    uart_printf(DBG_LVL_DBG, "W5500_Init() called!\n");
    uart_printf(DBG_LVL_DBG, "Registering W5500 callbacks...\r\n");

    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
}
