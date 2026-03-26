#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "main.h"

extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

extern SPI_HandleTypeDef hspi1;



void MX_SPI1_Init(void);
HAL_StatusTypeDef SPI_Transmit_Receive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint32_t TxSize,uint32_t RxSize);

#endif
