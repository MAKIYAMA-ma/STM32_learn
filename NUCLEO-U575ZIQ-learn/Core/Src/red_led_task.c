#include "main.h"
#include "app_freertos.h"
#include "red_led_task.h"

static uint8_t btnPress = 0;

void RedLEDTask(void)
{
    uint32_t itvList[] = {
        100,
        300,
        500,
        1000,
        2000
    };
    static uint32_t itvn = 0;

    if(btnPress) {
        btnPress = 0;
        // チャタリング防止 50msec後に再チェックして押下状態なら押されたと判断
        osDelay(50);
        if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_2) == GPIO_PIN_SET) {
            itvn = (itvn+1)%(sizeof(itvList)/sizeof(itvList[0]));
        }
    }

    HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_2);
    osDelay(itvList[itvn]);
}

void ButtonPressed(uint8_t val)
{
    btnPress = val;
}
