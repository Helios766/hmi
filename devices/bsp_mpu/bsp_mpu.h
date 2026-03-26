#ifndef __BSP_MPU_H__
#define __BSP_MPU_H__

#include "main.h"

void MPU_enable(void);
void MPU_disable(void);

void MPU_MemeoryRemap(uint32_t baseaddr, uint8_t regionnum, uint8_t size, uint8_t accessperm, uint8_t isshareable, uint8_t iscacheable, uint8_t isbufferable);

#endif