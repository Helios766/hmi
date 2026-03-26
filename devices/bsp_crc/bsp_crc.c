#include "bsp_crc.h"
#include <stdint.h>

CRC_HandleTypeDef hcrc;


void MX_CRC_Init(void)
{
    hcrc.Instance					  = CRC;
	hcrc.Init.CRCLength				  = CRC_POLYLENGTH_16B;         // 设置CRC宽度为16位
    hcrc.Init.DefaultPolynomialUse	  = DEFAULT_POLYNOMIAL_DISABLE; // 禁用默认多项式，使用0x8005
    hcrc.Init.DefaultInitValueUse 	  = DEFAULT_INIT_VALUE_DISABLE;  // 禁用默认初始值，手动设置初始值为 0xFFFF
	hcrc.Init.GeneratingPolynomial	  = 0x8005;                     // 设置多项式 0x8005 (对应 X^16 + X^15 + X^2 + 1)
    hcrc.Init.InitValue				  = 0xFFFF;                     // 设置初始值为 0xFFFF
    hcrc.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_BYTE;
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
    hcrc.InputDataFormat			  = CRC_INPUTDATA_FORMAT_BYTES;
    if (HAL_CRC_Init(&hcrc) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_CRC_MspInit(CRC_HandleTypeDef* crcHandle)
{
    if(crcHandle->Instance==CRC)    
    {
        __HAL_RCC_CRC_CLK_ENABLE();
    }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef* crcHandle)
{
    if(crcHandle->Instance==CRC)
    {
        /* Peripheral clock disable */
        __HAL_RCC_CRC_CLK_DISABLE();
    }
}

uint16_t hardware_crc16_calculate(uint8_t *pdata,uint32_t datalength)
{
    __HAL_CRC_DR_RESET(&hcrc);
    uint32_t raw = HAL_CRC_Calculate(&hcrc, (uint32_t*)pdata, datalength);
    return (uint16_t)raw; 
}