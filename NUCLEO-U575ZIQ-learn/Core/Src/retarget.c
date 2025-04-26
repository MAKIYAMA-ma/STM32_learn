#include "main.h"

#if 0   // printfを使うための関数
// これで printf() の出力先が UART1 になる
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

// scanf() を使う場合はこっちも必要
int __io_getchar(void)
{
    uint8_t ch;
    HAL_UART_Receive(&huart1, &ch, 1, HAL_MAX_DELAY);
    return ch;
}
#endif
