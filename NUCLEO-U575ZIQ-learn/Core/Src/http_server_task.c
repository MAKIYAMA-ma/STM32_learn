#include "main.h"
#include "app_freertos.h"
#include "cmsis_os.h"
#include "http_server_task.h"
#include "spi.h"
#include <string.h>
#include "socket.h"
#include "wizchip_conf.h"

#define SOCK_HTTP_SERVER 0
#define LISTEN_PORT 80

uint8_t recv_buf[1024];

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

void HTTPServerTaskProc(void *argument)
{
    int32_t ret;
    /* uint8_t client_ip[4]; */
    /* uint16_t client_port; */

    /* ソケットオープン */
    if (socket(SOCK_HTTP_SERVER, Sn_MR_TCP, LISTEN_PORT, 0) != SOCK_HTTP_SERVER)
    {
        uart_printf(DBG_LVL_DBG, "Socket open failed\r\n");
        for(;;);
    }

    /* リッスン開始 */
    if (listen(SOCK_HTTP_SERVER) != SOCK_OK)
    {
        uart_printf(DBG_LVL_DBG, "Listen failed\r\n");
        close(SOCK_HTTP_SERVER);
        for(;;);
    }
    uart_printf(DBG_LVL_DBG, "Listening on port %d\r\n", LISTEN_PORT);

    for(;;)
    {
        /* 接続待ち */
        if (getSn_SR(SOCK_HTTP_SERVER) == SOCK_ESTABLISHED)
        {
            uart_printf(DBG_LVL_DBG, "Client connected!\r\n");

            /* 受信待ち */
            ret = recv(SOCK_HTTP_SERVER, recv_buf, sizeof(recv_buf)-1);
            if (ret > 0)
            {
                recv_buf[ret] = '\0';  // 文字列終端
                uart_printf(DBG_LVL_DBG, "Received HTTP data:\r\n%s\r\n", recv_buf);

                /* HTTPリクエスト解析 */
                if (strstr((char*)recv_buf, "POST") != NULL)
                {
                    // POST
                    // TODO 内容の解析と処理

                    // HTTPレスポンス
                    const char http_response[] =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: 13\r\n"
                        "\r\n"
                        "Post received";

                    send(SOCK_HTTP_SERVER, (uint8_t*)http_response, sizeof(http_response)-1);
                    uart_printf(DBG_LVL_DBG, "Sent HTTP 200 OK response\r\n");
                }
                else
                {
                    // POST以外の場合はここ
                }

                // 一回ごとにソケットを閉じて再Listen
                disconnect(SOCK_HTTP_SERVER);
                uart_printf(DBG_LVL_DBG, "Client disconnected, listening again...\r\n");

                listen(SOCK_HTTP_SERVER);
            }
        }

        osDelay(10);  // 軽く待つ
    }
}
