#include "bsp_timer.h"
#include <stdint.h>


TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;

/* TIM3 init function */
void MX_TIM3_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim3.Instance               = TIM3;
    htim3.Init.Prescaler         = 0;                       // 无预分频
    htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;      // 向上计数模式
    htim3.Init.Period            = 65535;                   // 自动重装载值，决定计数器的周期
    htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;  // 时钟分频
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  // 自动重装载预装载使能
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;     // 主输出触发，重置
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;    // 主从模式，禁止
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode     = TIM_OCMODE_PWM1;         // PWM模式1，计数器小于比较值时输出高电平，大于比较值时输出低电平
    sConfigOC.Pulse      = 0;                       // 比较值，决定占空比，初始为0（0%占空比）
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;     // 输出极性，高电平有效
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;      // 关闭快速模式
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_TIM_MspPostInit(&htim3);
}

/* TIM4 init function */
void MX_TIM4_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
 
    htim4.Instance               = TIM4;
    htim4.Init.Prescaler         = 0;                       // 无预分频
    htim4.Init.CounterMode       = TIM_COUNTERMODE_UP;      // 向上计数模式
    htim4.Init.Period            = 65535;                   // 自动重装载值，决定计数器的周期
    htim4.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;  // 时钟分频
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  // 自动重装载预装载使能
    
    sConfig.EncoderMode          = TIM_ENCODERMODE_TI12;        // 编码器模式，使用TI1和TI2作为输入
    sConfig.IC1Polarity          = TIM_ICPOLARITY_RISING;       // 输入捕获1极性，捕获上升沿
    sConfig.IC1Selection         = TIM_ICSELECTION_DIRECTTI;    // 输入捕获1选择，直接连接到定时器输入
    sConfig.IC1Prescaler         = TIM_ICPSC_DIV1;              // 输入捕获1预分频，直接捕获不分频
    sConfig.IC1Filter            = 4;                           // 输入捕获1滤波，滤除噪声
    sConfig.IC2Polarity          = TIM_ICPOLARITY_RISING;       // 输入捕获2极性，捕获上升沿
    sConfig.IC2Selection         = TIM_ICSELECTION_DIRECTTI;    // 输入捕获2选择，直接连接到定时器输入
    sConfig.IC2Prescaler         = TIM_ICPSC_DIV1;              // 输入捕获2预分频，直接捕获不分频
    sConfig.IC2Filter            = 4;                           // 输入捕获2滤波，滤除噪声
    if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;         // 主输出触发，重置
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;        // 主从模式，禁止
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/* TIM6 init function */
void MX_TIM6_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 280-1;
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = 1000-1;
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

    if(tim_pwmHandle->Instance==TIM3)
    {
        /* TIM3 clock enable */
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* tim_encoderHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(tim_encoderHandle->Instance==TIM4)
    {
        /* TIM4 clock enable */
        __HAL_RCC_TIM4_CLK_ENABLE();

        __HAL_RCC_GPIOD_CLK_ENABLE();
        /**TIM4 GPIO Configuration
        PD12     ------> TIM4_CH1
        PD13     ------> TIM4_CH2
        */
        GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
    if(tim_baseHandle->Instance==TIM6)
    {
        /* TIM6 clock enable */
        __HAL_RCC_TIM6_CLK_ENABLE();

        /* TIM6 interrupt Init */
        HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
    }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(timHandle->Instance==TIM3)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**TIM3 GPIO Configuration
        PB5 ------> TIM3_CH2 */
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{
    if(tim_pwmHandle->Instance==TIM3)
    {
        __HAL_RCC_TIM3_CLK_DISABLE();
    }
}

void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef* tim_encoderHandle)
{
    if(tim_encoderHandle->Instance==TIM4)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM4_CLK_DISABLE();

        /**TIM4 GPIO Configuration
        PD12     ------> TIM4_CH1
        PD13     ------> TIM4_CH2
        */
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12|GPIO_PIN_13);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
    if(tim_baseHandle->Instance==TIM6)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM6_CLK_DISABLE();

        /* TIM6 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
    }
}

