#include "debug_led.h"
#include <stdint.h>

uint16_t led_on_time;
uint16_t led_off_time;
uint16_t led_counter;

void debug_led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    led_counter = 0;
    debug_led_set(500, 500);

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
    
    /*Configure GPIO pin : DEBUGLED_Pin */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);;
}

void debug_led_set(uint16_t on_time,uint16_t off_time)
{
    led_on_time = on_time;
    led_off_time = off_time;
}

void LED_Debug_Update(void)
{
    if(++led_counter > (led_on_time + led_off_time))
    {
        led_counter = 0;
    }
    if(led_counter < led_on_time)
    {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
    }
}