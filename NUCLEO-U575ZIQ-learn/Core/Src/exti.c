#include "main.h"
#include "red_led_task.h"

/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_13) {
        // on board button
        /* uart_printf(DBG_LVL_DBG, "Button rising edge\n"); */
        ButtonPressed(1);
    }
}

#if 0
/**
  * @brief  EXTI line falling detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_13) {
        // on board button
        uart_printf(DBG_LVL_DBG, "Button falling edge\n");
    }
}
#endif
