#include "debug_usart.h"
#include "bsp_usart.h"
#include "stdio.h"

void DEBUG_USART_Init(void)
{
    MX_USART1_UART_Init();
}

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

int fgetc(FILE *f)
{
    uint8_t ch = 0;
    HAL_UART_Receive(&huart1, &ch, 1, HAL_MAX_DELAY);
    return ch;
}