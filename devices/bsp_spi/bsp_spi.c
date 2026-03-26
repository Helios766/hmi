#include "bsp_spi.h"

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

void MX_SPI1_Init(void)
{
	hspi1.Instance						  = SPI1;
	hspi1.Init.Mode						  = SPI_MODE_MASTER;			// 主机模式
	hspi1.Init.Direction				  = SPI_DIRECTION_2LINES;       // 全双工
	hspi1.Init.DataSize					  = SPI_DATASIZE_8BIT;          // 数据大小8位
	hspi1.Init.CLKPolarity				  = SPI_POLARITY_LOW;          	// 时钟空闲低电平
	hspi1.Init.CLKPhase					  = SPI_PHASE_1EDGE;            // 数据采样在第一个时钟沿
	hspi1.Init.NSS						  = SPI_NSS_HARD_OUTPUT;		// 硬件NSSs输出
	
	//PLL3P = 72MHz	/2 = 36MHz
	hspi1.Init.BaudRatePrescaler		  = SPI_BAUDRATEPRESCALER_2;    // 波特率预分频值
	
	hspi1.Init.FirstBit					  = SPI_FIRSTBIT_MSB;           // 数据传输从MSB位开始
	hspi1.Init.TIMode					  = SPI_TIMODE_DISABLE;         // 禁用TI模式
	hspi1.Init.CRCCalculation			  = SPI_CRCCALCULATION_ENABLE;  // 使能CRC校验
	hspi1.Init.CRCPolynomial			  = 0x0;                       // CRC多项式
	hspi1.Init.NSSPMode					  = SPI_NSS_PULSE_DISABLE;      // 禁用NSS脉冲模式
	hspi1.Init.NSSPolarity				  = SPI_NSS_POLARITY_LOW;       // NSS低有效
	hspi1.Init.FifoThreshold			  = SPI_FIFO_THRESHOLD_02DATA;  // FIFO阈值为1字节
	hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;    // 发送CRC初始化模式全0
	hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;    // 接收CRC初始化模式全0
	hspi1.Init.MasterSSIdleness		 	  = SPI_MASTER_SS_IDLENESS_00CYCLE;             // 主机SS空闲时间0个时钟周期
	hspi1.Init.MasterInterDataIdleness	  = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;      // 主机数据间隔时间0个时钟周期
	hspi1.Init.MasterReceiverAutoSusp	  = SPI_MASTER_RX_AUTOSUSP_DISABLE;             // 禁用主机接收自动挂起
	hspi1.Init.MasterKeepIOState		  = SPI_MASTER_KEEP_IO_STATE_ENABLE;           // 主机保持IO状态
	hspi1.Init.IOSwap					  = SPI_IO_SWAP_DISABLE;                        // 禁用IO交换
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(spiHandle->Instance==SPI1)
	{
		/* SPI1 clock enable */
		__HAL_RCC_SPI1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**SPI1 GPIO Configuration
		PA15     ------> SPI1_NSS
		PD7     ------> SPI1_MOSI
		PB3     ------> SPI1_SCK
		PB4     ------> SPI1_MISO
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
//		
//		/* SPI1 DMA Init */
//		/* SPI1_RX Init */
//		hdma_spi1_rx.Instance				  = DMA1_Stream0;
//		hdma_spi1_rx.Init.Request			  = DMA_REQUEST_SPI1_RX;
//		hdma_spi1_rx.Init.Direction			  = DMA_PERIPH_TO_MEMORY;
//		hdma_spi1_rx.Init.PeriphInc			  = DMA_PINC_DISABLE;
//		hdma_spi1_rx.Init.MemInc			  = DMA_MINC_ENABLE;
//		hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//		hdma_spi1_rx.Init.MemDataAlignment	  = DMA_MDATAALIGN_BYTE;
//		hdma_spi1_rx.Init.Mode				  = DMA_NORMAL;
//		hdma_spi1_rx.Init.Priority			  = DMA_PRIORITY_LOW;
//		hdma_spi1_rx.Init.FIFOMode			  = DMA_FIFOMODE_ENABLE;
//		hdma_spi1_rx.Init.FIFOThreshold		  = DMA_FIFO_THRESHOLD_FULL;
//		hdma_spi1_rx.Init.MemBurst			  = DMA_MBURST_SINGLE;
//		hdma_spi1_rx.Init.PeriphBurst		  = DMA_PBURST_SINGLE;
//		if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
//		{
//			Error_Handler();
//		}
//		if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
//		{
//			Error_Handler();
//		}
//		__HAL_LINKDMA(spiHandle,hdmarx,hdma_spi1_rx);

//		/* SPI1_TX Init */
//		hdma_spi1_tx.Instance				  = DMA1_Stream1;
//		hdma_spi1_tx.Init.Request			  = DMA_REQUEST_SPI1_TX;
//		hdma_spi1_tx.Init.Direction			  = DMA_MEMORY_TO_PERIPH;
//		hdma_spi1_tx.Init.PeriphInc			  = DMA_PINC_DISABLE;
//		hdma_spi1_tx.Init.MemInc			  = DMA_MINC_ENABLE;
//		hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//		hdma_spi1_tx.Init.MemDataAlignment	  = DMA_MDATAALIGN_BYTE;
//		hdma_spi1_tx.Init.Mode				  = DMA_NORMAL;
//		hdma_spi1_tx.Init.Priority			  = DMA_PRIORITY_LOW;
//		hdma_spi1_tx.Init.FIFOMode			  = DMA_FIFOMODE_DISABLE;
//		if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
//		{
//			Error_Handler();
//		}
//		if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
//		{
//			Error_Handler();
//		}
//		__HAL_LINKDMA(spiHandle,hdmatx,hdma_spi1_tx);

//		/* SPI1 interrupt Init */
//		HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
//		HAL_NVIC_EnableIRQ(SPI1_IRQn);
		
	}
}

//void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
//{
//	if(spiHandle->Instance==SPI1)
//	{
//		/* Peripheral clock disable */
//		__HAL_RCC_SPI1_CLK_DISABLE();

//		/**SPI1 GPIO Configuration
//		PA15     ------> SPI1_NSS
//		PD7     ------> SPI1_MOSI
//		PB3     ------> SPI1_SCK
//		PB4     ------> SPI1_MISO
//		*/
//		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);
//		HAL_GPIO_DeInit(GPIOD, GPIO_PIN_7);
//		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4);
//		
//		/* SPI1 DMA DeInit */
//		HAL_DMA_DeInit(spiHandle->hdmarx);
//		HAL_DMA_DeInit(spiHandle->hdmatx);

//		/* SPI1 interrupt Deinit */
//		HAL_NVIC_DisableIRQ(SPI1_IRQn);
//	}
//}


/**
  * @brief Handle SPI Communication Timeout.
  * @param hspi: pointer to a SPI_HandleTypeDef structure that contains
  *              the configuration information for SPI module.
  * @param Flag: SPI flag to check
  * @param Status: flag state to check
  * @param Timeout: Timeout duration
  * @param Tickstart: Tick start value
  * @retval HAL status
  */
HAL_StatusTypeDef SPI_WaitOnFlagUntilTimeout(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus Status,
                                                    uint32_t Tickstart, uint32_t Timeout)
{
    /* Wait until flag is set */
    while ((__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) == Status)
    {
        /* Check for the Timeout */
        if ((((HAL_GetTick() - Tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
            return HAL_TIMEOUT;
        }
    }
    return HAL_OK;
}


/**
 * @brief  Close Transfer and clear flags.
 * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
 *               the configuration information for SPI module.
 * @retval HAL_ERROR: if any error detected
 *         HAL_OK: if nothing detected
 */
 void SPI_Close_Transfer(SPI_HandleTypeDef *hspi)
{
    uint32_t itflag = hspi->Instance->SR;

    __HAL_SPI_CLEAR_EOTFLAG(hspi);
    __HAL_SPI_CLEAR_TXTFFLAG(hspi);

    /* Disable SPI peripheral */
    __HAL_SPI_DISABLE(hspi);

    /* Disable ITs */
    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

    /* Disable Tx DMA Request */
    CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

    /* Report UnderRun error for non RX Only communication */
    if (hspi->State != HAL_SPI_STATE_BUSY_RX)
    {
        if ((itflag & SPI_FLAG_UDR) != 0UL)
        {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_UDR);
            __HAL_SPI_CLEAR_UDRFLAG(hspi);
        }
    }

    /* Report OverRun error for non TX Only communication */
    if (hspi->State != HAL_SPI_STATE_BUSY_TX)
    {
        if ((itflag & SPI_FLAG_OVR) != 0UL)
        {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_OVR);
            __HAL_SPI_CLEAR_OVRFLAG(hspi);
        }
    }

    /* SPI Mode Fault error interrupt occurred -------------------------------*/
    if ((itflag & SPI_FLAG_MODF) != 0UL)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_MODF);
        __HAL_SPI_CLEAR_MODFFLAG(hspi);
    }

    /* SPI Frame error interrupt occurred ------------------------------------*/
    if ((itflag & SPI_FLAG_FRE) != 0UL)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FRE);
        __HAL_SPI_CLEAR_FREFLAG(hspi);
    }

    hspi->TxXferCount = (uint16_t)0UL;
    hspi->RxXferCount = (uint16_t)0UL;
}


/**
  * @brief  Transmit and Receive an amount of data in blocking mode.
  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
  *                  the configuration information for SPI module.
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size   : amount of data to be sent and received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */

HAL_StatusTypeDef SPI_Transmit_Receive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint32_t TxSize,uint32_t RxSize)
{
    HAL_StatusTypeDef errorcode = HAL_OK;
    uint32_t    tickstart;
    uint32_t    Timeout = 1000;
    uint32_t    initial_TxXferCount;
    uint32_t    initial_RxXferCount;

    /* Check Direction parameter */
    assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

    /* Process Locked */
    __HAL_LOCK(hspi);

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
    if (hspi->State != HAL_SPI_STATE_BUSY_RX)
    {
        hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
    }

    /* Set the transaction information */
    hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
    hspi->pRxBuffPtr  = (uint8_t *)pRxData;
    hspi->pTxBuffPtr  = (uint8_t *)pTxData;

    /*Init field not used in handle to zero */
    hspi->RxISR       = NULL;
    hspi->TxISR       = NULL;

    // 不使用硬件 TSIZE 控制，此处设置为0，即不限制传输的数据长度
    MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, 0);

    __HAL_SPI_ENABLE(hspi);

    if (hspi->Init.Mode == SPI_MODE_MASTER)
    {
        /* Master transfer start */
        SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
    }

    /* 先发送命令和地址，长度为 TxSize */   
    initial_TxXferCount = TxSize;
    initial_RxXferCount = TxSize;  
    hspi->pRxBuffPtr  = (uint8_t *)pRxData;
    hspi->pTxBuffPtr  = (uint8_t *)pTxData;

    while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
    {
        /* check TXP flag */
        if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL))
        {
            *((__IO uint8_t *)&hspi->Instance->TXDR) = *((uint8_t *)hspi->pTxBuffPtr);
            hspi->pTxBuffPtr += sizeof(uint8_t);
            initial_TxXferCount--;         
        }
        /* Wait until RXWNE/FRLVL flag is reset */
        if (((hspi->Instance->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)) != 0UL) && (initial_RxXferCount > 0UL))
        {
            *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
            hspi->pRxBuffPtr += sizeof(uint8_t);
            initial_RxXferCount --;         
        }
    }

    /* 开始读取数据，长度为 RxSize */   
    initial_TxXferCount = RxSize ;
    initial_RxXferCount = RxSize ;  
    hspi->pRxBuffPtr  = (uint8_t *)pRxData;
    hspi->pTxBuffPtr  = (uint8_t *)pTxData;

    /* Transmit and Receive data in 8 Bit mode */
    while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
    {
        /* check TXP flag */
        if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL))
        {
            if ((initial_TxXferCount > 3UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_03DATA))
            {
                *((__IO uint32_t *)&hspi->Instance->TXDR) = *((uint32_t *)hspi->pTxBuffPtr);
                hspi->pTxBuffPtr += sizeof(uint32_t);
                initial_TxXferCount -= (uint16_t)4UL;
            }
            else if ((initial_TxXferCount > 1UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
            {
                *((__IO uint16_t *)&hspi->Instance->TXDR) = *((uint16_t *)hspi->pTxBuffPtr);
                hspi->pTxBuffPtr += sizeof(uint16_t);
                initial_TxXferCount -= (uint16_t)2UL;         
            }
            else
            {
                *((__IO uint8_t *)&hspi->Instance->TXDR) = *((uint8_t *)hspi->pTxBuffPtr);
                hspi->pTxBuffPtr += sizeof(uint8_t);
                initial_TxXferCount--;         
            }
        }

        /* Wait until RXWNE/FRLVL flag is reset */
        if (((hspi->Instance->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)) != 0UL) && (initial_RxXferCount > 0UL))
        {
            if ((hspi->Instance->SR & SPI_FLAG_RXWNE) != 0UL)
            {
                *((uint32_t *)hspi->pRxBuffPtr) = *((__IO uint32_t *)&hspi->Instance->RXDR);
                hspi->pRxBuffPtr += sizeof(uint32_t);  
                initial_RxXferCount -= (uint16_t)4UL;		          
            }
            else if ((hspi->Instance->SR & SPI_FLAG_FRLVL) > SPI_RX_FIFO_1PACKET)
            {
                *((uint16_t *)hspi->pRxBuffPtr) = *((__IO uint16_t *)&hspi->Instance->RXDR);
                hspi->pRxBuffPtr += sizeof(uint16_t);
                initial_RxXferCount -= (uint16_t)2UL;	         
            }
            else
            {
                *((uint8_t *)hspi->pRxBuffPtr) = *((__IO uint8_t *)&hspi->Instance->RXDR);
                hspi->pRxBuffPtr += sizeof(uint8_t);
                initial_RxXferCount --;         
            }
        }
        /* Timeout management */
        if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
        {
            /* Call standard close procedure with error check */
            SPI_Close_Transfer(hspi);

            /* Process Unlocked */
            __HAL_UNLOCK(hspi);

            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
            hspi->State = HAL_SPI_STATE_READY;

            // printf ("\r\ninitial_RxXferCount:%d\r\n",initial_RxXferCount);

            return HAL_ERROR;
        }
    }

    SET_BIT((hspi)->Instance->CR1 , SPI_CR1_CSUSP); // 请求挂起SPI传输

    /* 等待SPI挂起 */
    if (SPI_WaitOnFlagUntilTimeout(hspi, SPI_FLAG_SUSP, RESET, tickstart, Timeout) != HAL_OK)
    {
        SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
    }
    SPI_Close_Transfer(hspi);   /* Call standard close procedure with error check */

    SET_BIT((hspi)->Instance->IFCR , SPI_IFCR_SUSPC);  // 清除挂起标志位

    /* Process Unlocked */
    __HAL_UNLOCK(hspi);

    hspi->State = HAL_SPI_STATE_READY;

    if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
    {
        return HAL_ERROR;
    }
    return errorcode;
}





