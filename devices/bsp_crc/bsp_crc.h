#ifndef _BSP_CRC_H_
#define _BSP_CRC_H_

#include "main.h"

extern CRC_HandleTypeDef hcrc;
void MX_CRC_Init(void);

uint16_t hardware_crc16_calculate(uint8_t *pdata,uint32_t datalength);

#endif
