#ifndef _BSP_BUZZER_H_
#define _BSP_BUZZER_H_

#include "main.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;

void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM6_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

#endif
