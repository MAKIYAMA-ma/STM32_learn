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
    if (xSemaphoreTake(spi1RxDoneSem, portMAX_DELAY) != pdTRUE) {
        uart_printf(DBG_LVL_DBG, "SPI Master DMA tx timeout\n");
    }
}

void W5500_WriteBuff(uint8_t* buff, uint16_t len)
{
    if(HAL_SPI_Transmit_DMA(&hspi1, buff, len) != HAL_OK) {
        // TODO エラー処理
        return;
    }
    if (xSemaphoreTake(spi1TxDoneSem, portMAX_DELAY) != pdTRUE) {
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

    HAL_GPIO_WritePin(W5500_RESET_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
    osDelay(1000);
    HAL_GPIO_WritePin(W5500_RESET_Port, W5500_RESET_Pin, GPIO_PIN_SET);

    uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};
    wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

    wiz_NetInfo net_info = {
        .mac  = { 0xEA, 0x11, 0x22, 0x33, 0x44, 0xEA },
        .dhcp = NETINFO_STATIC,
        .ip = {192, 168, 0, 100},
        .sn = {255, 255, 255, 0},
        .gw = {192, 168, 0, 101},
    };
    // set MAC address before using DHCP
    wizchip_setnetinfo(&net_info);

    uint8_t ver = getVERSIONR();  // WIZnet ioLibrary を使っている前提
    if (ver == 0x04) {
        uart_printf(DBG_LVL_DBG, "W5500 Version: 0x%02X (OK)\n", ver);
    } else {
        uart_printf(DBG_LVL_ERROR, "W5500 Version: 0x%02X (NG)\n", ver);
    }
}

void HTTPServerTaskProc(void *argument)
{
    int32_t ret;
    /* uint8_t client_ip[4]; */
    /* uint16_t client_port; */

    spi1TxDoneSem = xSemaphoreCreateBinary();
    spi1RxDoneSem = xSemaphoreCreateBinary();
    if (spi1TxDoneSem == NULL || spi1RxDoneSem == NULL) {
        uart_printf(DBG_LVL_ERROR, "Failed to create SPI Master semaphore\n");
        vTaskDelete(NULL);
    }

    W5500_Init();

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
