#include "bsp_i2c.h"

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;
DMA_HandleTypeDef hdma_i2c3_rx;


/* I2C1 init function */
void MX_I2C1_Init(void)
{
	hi2c1.Instance 				= I2C1;
	hi2c1.Init.Timing 			= 0x20B0CCFF;
	hi2c1.Init.OwnAddress1 		= 0;
	hi2c1.Init.AddressingMode 	= I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode 	= I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 		= 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode 	= I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode 	= I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Analogue filter
	*/
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Digital filter
	*/
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
	{
		Error_Handler();
	}
}

void MX_I2C3_Init(void)
{
	hi2c3.Instance			    = I2C3;
	hi2c3.Init.Timing		    = 0x00601C5C;
	hi2c3.Init.OwnAddress1	    = 0;
	hi2c3.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
	hi2c3.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
	hi2c3.Init.OwnAddress2	    = 0;
	hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c3.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
	hi2c3.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c3) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Analogue filter
	*/
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Digital filter
	*/
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
	{
		Error_Handler();
	}

	/** I2C Enable Fast Mode Plus
	*/
	HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C3);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if(i2cHandle->Instance==I2C1)
	{
		/** Initializes the peripherals clock
		*/
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
		PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
		    Error_Handler();
		}

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**I2C1 GPIO Configuration
		PB6     ------> I2C1_SCL
		PB7     ------> I2C1_SDA
		*/
		GPIO_InitStruct.Pin	 	  = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode	  = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull	  = GPIO_PULLUP;
		GPIO_InitStruct.Speed	  = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* I2C1 clock enable */
		__HAL_RCC_I2C1_CLK_ENABLE();
	}
	else if(i2cHandle->Instance==I2C3)
	{
		/** Initializes the peripherals clock
		*/
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
		PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
		    Error_Handler();
		}

		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**I2C3 GPIO Configuration
		PC9     ------> I2C3_SDA
		PA8     ------> I2C3_SCL
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		    /* I2C3 clock enable */
		__HAL_RCC_I2C3_CLK_ENABLE();

		/* I2C3 DMA Init */
		/* I2C3_RX Init */
		hdma_i2c3_rx.Instance				  = DMA1_Stream4;
		hdma_i2c3_rx.Init.Request			  = DMA_REQUEST_I2C3_RX;
		hdma_i2c3_rx.Init.Direction			  = DMA_PERIPH_TO_MEMORY;
		hdma_i2c3_rx.Init.PeriphInc			  = DMA_PINC_DISABLE;
		hdma_i2c3_rx.Init.MemInc			  = DMA_MINC_ENABLE;
		hdma_i2c3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_i2c3_rx.Init.MemDataAlignment	  = DMA_MDATAALIGN_BYTE;
		hdma_i2c3_rx.Init.Mode				  = DMA_NORMAL;
		hdma_i2c3_rx.Init.Priority			  = DMA_PRIORITY_LOW;
		hdma_i2c3_rx.Init.FIFOMode			  = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_i2c3_rx) != HAL_OK)
		{
		  Error_Handler();
		}

		__HAL_LINKDMA(i2cHandle,hdmarx,hdma_i2c3_rx);
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
	if(i2cHandle->Instance==I2C1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_I2C1_CLK_DISABLE();

		/**I2C1 GPIO Configuration
		PB6     ------> I2C1_SCL
		PB7     ------> I2C1_SDA
		*/
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
	}
	else if(i2cHandle->Instance==I2C3)
	{
		/* Peripheral clock disable */
		__HAL_RCC_I2C3_CLK_DISABLE();

		/**I2C3 GPIO Configuration
		PC9     ------> I2C3_SDA
		PA8     ------> I2C3_SCL
		*/
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
		
		HAL_DMA_DeInit(i2cHandle->hdmarx);
	}
}



