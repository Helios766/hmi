#include "main.h"
#include "stm32h7xx_it.h"

//#include "Keyboard.h"

extern LTDC_HandleTypeDef hltdc;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim6;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim17;
extern DMA_HandleTypeDef hdma_i2c3_rx;
extern I2C_HandleTypeDef hi2c3;

void NMI_Handler(void)      // 这个函数是当系统发生一个不可屏蔽的中断时被调用的，比如电源故障或者时钟故障等
{
    HAL_RCC_NMI_IRQHandler();
    while (1)
    {

    }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)    // 这个函数是当CPU遇到一个无法处理的异常或者错误时被调用的，比如访问了一个无效的内存地址或者执行了一个非法的指令等
{
    while (1)
    {

    }
}

void MemManage_Handler(void)    // 这个函数是当CPU访问了一个受保护的内存区域或者发生了内存管理错误时被调用的
{
    while (1)
    {

    }
}

void BusFault_Handler(void)     // 这个函数是当CPU访问了一个无效的内存地址或者发生了总线错误时被调用的
{
    while (1)
    {
    }
}


void UsageFault_Handler(void)   // 这个函数是当CPU执行了一个无效的指令或者发生了其他使用错误时被调用的
{
    while (1)
    {
    }
}

void SVC_Handler(void)
{
    
}

void DebugMon_Handler(void)
{
    
}

void PendSV_Handler(void)
{

}

void SysTick_Handler(void)
{

}

/********************USART1 DMA相关中断***********************/
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}

void DMA1_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void DMA1_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

/********************I3C DMA相关中断 与键盘相关***********************/
//void DMA1_Stream4_IRQHandler(void)
//{
//    HAL_DMA_IRQHandler(&hdma_i2c3_rx);
//}

//void DMAMUX1_OVR_IRQHandler(void)
//{
//    HAL_DMAEx_MUX_IRQHandler(&hdma_i2c3_rx);
//}

//void EXTI1_IRQHandler(void)
//{
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
//	if(hi2c3.State == HAL_I2C_STATE_READY)
//	{
//		TCA8418_DMA_ReadKeyEvent();
//	}
//}

/********************SPI1 DMA相关中断***********************/
void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&hspi1);
}

void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi1_rx);
}

void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

/********************TIM6 中断 用于无源蜂鸣器***********************/
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}

/********************LTDC 中断 用于显示屏***********************/
void LTDC_IRQHandler(void)
{
    HAL_LTDC_IRQHandler(&hltdc);
}

/********************TIM17 中断 用于HAL库时基***********************/
void TIM17_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim17);
}
