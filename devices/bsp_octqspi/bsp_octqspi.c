#include "bsp_octqspi.h"

OSPI_HandleTypeDef hospi1;

/* OCTOSPI1 init function */
void MX_OCTOSPI1_Init(void)
{
	OSPIM_CfgTypeDef sOspiManagerCfg = {0};
	
	hospi1.Instance					  = OCTOSPI1;
	hospi1.Init.ClockPrescaler		  = 2;		// 时钟分频值，将OSPI内核时钟进行 2 分频得到OSPI通信驱动时钟
	hospi1.Init.FifoThreshold		  = 8;		// FIFO阈值
	hospi1.Init.DualQuad			  = HAL_OSPI_DUALQUAD_DISABLE;	// 禁止双OSPI模式
	hospi1.Init.MemoryType			  = HAL_OSPI_MEMTYPE_MICRON;	// 存储器模式，只有8线模式才会用到
	hospi1.Init.DeviceSize			  = 23;		// flash大小，采用是8M字节的W25Q64，这里设置为23，即2^23
	hospi1.Init.ChipSelectHighTime	  = 1;		// 片选保持高电平的时间
	hospi1.Init.FreeRunningClock	  = HAL_OSPI_FREERUNCLK_DISABLE;	// 禁止自由时钟模式
	hospi1.Init.ClockMode			  = HAL_OSPI_CLOCK_MODE_3;			// 模式3
	hospi1.Init.WrapSize			  = HAL_OSPI_WRAP_NOT_SUPPORTED;	// 不使用 wrap-size
	hospi1.Init.SampleShifting		  = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;		// 半个CLK周期之后进行采样
	hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;			// 不使用数据保持功能
	hospi1.Init.ChipSelectBoundary	  = 0;		// 禁止片选边界功能
	hospi1.Init.DelayBlockBypass	  = HAL_OSPI_DELAY_BLOCK_BYPASSED;	// 延时模块旁路
	hospi1.Init.MaxTran				  = 0;		// 禁止通信管理功能
	hospi1.Init.Refresh				  = 0;		// 禁止刷新功能
	if (HAL_OSPI_Init(&hospi1) != HAL_OK)
	{
		Error_Handler();
	}
	sOspiManagerCfg.ClkPort			  = 1;		// OSPI引脚分配管理器设置，使用 Port1 的 CLK
	sOspiManagerCfg.NCSPort			  = 1;		// OSPI引脚分配管理器设置，使用 Port1 的 NCS
	sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;		// OSPI引脚分配管理器设置，使用 Port1 的低4位引脚，IO[3:0]
	if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_OSPI_MspInit(OSPI_HandleTypeDef* ospiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if(ospiHandle->Instance==OCTOSPI1)
	{
		/** Initializes the peripherals clock
		*/
		PeriphClkInitStruct.PeriphClockSelection	= RCC_PERIPHCLK_OSPI;
		PeriphClkInitStruct.OspiClockSelection		= RCC_OSPICLKSOURCE_D1HCLK;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
			Error_Handler();
		}

		/* OCTOSPI1 clock enable */
		__HAL_RCC_OCTOSPIM_CLK_ENABLE();
		__HAL_RCC_OSPI1_CLK_ENABLE();

		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**OCTOSPI1 GPIO Configuration
		PB2     ------> OCTOSPIM_P1_CLK
		PC11     ------> OCTOSPIM_P1_NCS
		PC3_C     ------> OCTOSPIM_P1_IO0
		PC10     ------> OCTOSPIM_P1_IO1
		PE2     ------> OCTOSPIM_P1_IO2
		PA1     ------> OCTOSPIM_P1_IO3
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_10|GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef* ospiHandle)
{
	if(ospiHandle->Instance == OCTOSPI1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_OCTOSPIM_CLK_DISABLE();
		__HAL_RCC_OSPI1_CLK_DISABLE();

		/**OCTOSPI1 GPIO Configuration
		PE2     ------> OCTOSPIM_P1_IO2
		PC3_C     ------> OCTOSPIM_P1_IO0
		PA1     ------> OCTOSPIM_P1_IO3
		PB2     ------> OCTOSPIM_P1_CLK
		PC10     ------> OCTOSPIM_P1_IO1
		PC11     ------> OCTOSPIM_P1_NCS
		*/
		HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);

		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3|GPIO_PIN_10|GPIO_PIN_11);

		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);
	}
}
