#include "main.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

// リングバッファサイズ
#define UART_TX_RINGBUF_SIZE   512
#define UART_RX_LINEBUF_SIZE   128

typedef void (*CommandCallback)(const uint8_t *params);
typedef struct {
    const char *commandStr;
    CommandCallback callback;
} ST_COMMAND_ENTRY;

void HandleDbglvl(const uint8_t *params)
{
    int val = atoi((char *)params);
    if(val < 0) val = 0;
    if(val > DBG_LVL_MAX) val = DBG_LVL_MAX;
    g_user_setting.dbg_lvl = val;
    SaveUserSetting(&g_user_setting);
}

static ST_COMMAND_ENTRY commands[] = {
    {"DBGLVL", HandleDbglvl},
};
#define NUM_COMMANDS (sizeof(commands)/sizeof(commands[0]))

// 送信用リングバッファ
static uint8_t uartTxRingBuf[UART_TX_RINGBUF_SIZE];
static uint8_t uartTxTmpBuf[UART_RX_LINEBUF_SIZE];
static volatile uint16_t uartTxHead = 0;
static volatile uint16_t uartTxTail = 0;
static volatile uint8_t uartTxBusy = 0;

// 受信用リングバッファ
static uint8_t uartRxByteBuf;
static uint8_t uartRxLineBuf[UART_RX_LINEBUF_SIZE];
static volatile uint16_t uartRxLineLen = 0;

void uart_init()
{
    HAL_UART_Receive_IT(&huart1, &uartRxByteBuf, 1); // 1バイトずつ受信開始
}

void uart_printf(EN_DBG_LEVEL lvl, const char *fmt, ...)
{
    va_list args;

    if(g_user_setting.dbg_lvl < (uint8_t)lvl) return;

    va_start(args, fmt);
    int len = vsnprintf((char *)uartTxTmpBuf, sizeof(uartTxTmpBuf), fmt, args);
    va_end(args);

    taskENTER_CRITICAL();
    for (int i = 0; i < len; i++) {
        uint16_t nextHead = (uartTxHead + 1) % UART_TX_RINGBUF_SIZE;
        if (nextHead == uartTxTail) {
            // バッファ満杯、捨てる（または待つ設計もできる）
            break;
        }
        uartTxRingBuf[uartTxHead] = uartTxTmpBuf[i];
        uartTxHead = nextHead;
    }

    if (!uartTxBusy) {
        uartTxBusy = 1;
        uint8_t data = uartTxRingBuf[uartTxTail];
        uartTxTail = (uartTxTail + 1) % UART_TX_RINGBUF_SIZE;
        HAL_UART_Transmit_DMA(&huart1, &data, 1);
    }
    taskEXIT_CRITICAL();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        if (uartTxTail != uartTxHead) {
            uint8_t data = uartTxRingBuf[uartTxTail];
            uartTxTail = (uartTxTail + 1) % UART_TX_RINGBUF_SIZE;
            HAL_UART_Transmit_DMA(huart, &data, 1);
        } else {
            uartTxBusy = 0;
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        uint8_t ch = uartRxByteBuf;

        if (uartRxLineLen < UART_RX_LINEBUF_SIZE - 1) {
            uartRxLineBuf[uartRxLineLen++] = ch;
            if (ch == '\n') {
                uartRxLineBuf[uartRxLineLen] = '\0'; // null terminate
                // TODO 一行受信完了
                uart_printf(DBG_LVL_ERROR, "Received Line: %s", uartRxLineBuf);
                for (int i = 0; i < NUM_COMMANDS; i++) {
                    size_t cmdLen = strlen(commands[i].commandStr);
                    if (strncmp((char *)uartRxLineBuf, commands[i].commandStr, cmdLen) == 0) {
                        const uint8_t *paramPart = uartRxLineBuf + cmdLen;
                        // スペースを飛ばすなどしたければここで追加もOK
                        commands[i].callback(paramPart);
                        return;
                    }
                }
                uartRxLineLen = 0;
            }
        } else {
            // バッファオーバーフロー、クリア
            uartRxLineLen = 0;
        }

        HAL_UART_Receive_IT(huart, &uartRxByteBuf, 1); // 次を受信
    }
}
