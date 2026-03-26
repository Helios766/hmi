#include "bsp_ltdc.h"

LTDC_HandleTypeDef hltdc;



void MX_LTDC_Init(void)
{
	LTDC_LayerCfgTypeDef pLayerCfg = {0};

	hltdc.Instance				  = LTDC;
	hltdc.Init.HSPolarity		  = LTDC_HSPOLARITY_AL;     // 低电平有效
	hltdc.Init.VSPolarity		  = LTDC_VSPOLARITY_AL;     // 低电平有效
	hltdc.Init.DEPolarity		  = LTDC_DEPOLARITY_AL;     // 低电平有效，要注意的是，很多面板都是高电平有效，但是7B0需要设置成低电平才能正常显示
	hltdc.Init.PCPolarity  		  = LTDC_PCPOLARITY_IPC;    // 正常时钟信号

	hltdc.Init.HorizontalSync 		= HSW - 1;									// ¸ù¾ÝÆÁÄ»ÉèÖÃ²ÎÊý¼´¿É
	hltdc.Init.VerticalSync 		= VSW	-1 ;
	hltdc.Init.AccumulatedHBP		= HBP + HSW -1;
	hltdc.Init.AccumulatedVBP 		= VBP + VSW -1;
	hltdc.Init.AccumulatedActiveW 	= LCD_WIDTH  + HSW + HBP -1;
	hltdc.Init.AccumulatedActiveH 	= LCD_HEIGHT + VSW + VBP -1;
	hltdc.Init.TotalWidth 			= LCD_WIDTH  + HSW + HBP + HFP - 1; 
	hltdc.Init.TotalHeigh 			= LCD_HEIGHT + VSW + VBP + VFP - 1;
	
	hltdc.Init.Backcolor.Red 		= 0;	// 初始背景色，R	
	hltdc.Init.Backcolor.Green 	    = 0;    // 初始背景色，G	
	hltdc.Init.Backcolor.Blue 		= 0;	// 初始背景色，B
	if (HAL_LTDC_Init(&hltdc) != HAL_OK)    // 初始化LTDC参数
	{
		Error_Handler();
	}
/*---------------------------------- layer0 显示配置 --------------------------------*/	
	pLayerCfg.WindowX0		  = 0;                              // 水平起点
	pLayerCfg.WindowX1		  = LCD_WIDTH;                      // 水平终点
	pLayerCfg.WindowY0		  = 0;                              // 垂直起点
	pLayerCfg.WindowY1		  = LCD_HEIGHT;                     // 垂直终点

    pLayerCfg.ImageWidth	  = LCD_WIDTH;                      // 显示区域宽度
	pLayerCfg.ImageHeight	  = LCD_HEIGHT;                     // 显示区域高度	
	
#if  (LCD_PIXEL_SIZE == 4)
	pLayerCfg.PixelFormat	  = LTDC_PIXEL_FORMAT_ARGB888;       // 颜色格式	
#elif (LCD_PIXEL_SIZE == 3)
    pLayerCfg.PixelFormat	  = LTDC_PIXEL_FORMAT_RGB888;       // 颜色格式	
#elif (LCD_PIXEL_SIZE == 2)
	pLayerCfg.PixelFormat	  = LTDC_PIXEL_FORMAT_RGB565;       // 颜色格式	
#endif
	
	pLayerCfg.FBStartAdress	  = FRAME_BUFFER0_ADDR;				// 显存地址,使用7B0内部的AXI SRAM作为显存，起始地址0x24000000，大小为1M
                                                                // layer0 显存大小等于 = LCD_WIDTH * LCD_WIDTH * BytesPerPixel_0（每个像素所占字节大小）
                                                                // 因为 AXI SRAM 的大小只有1M，用户设置的区域一定不能超过这个值！	
                                                                // 在本程序中，480*272的屏幕，使用RGB565格式，需要显存 480*272*2 = 261120 字节，大约255KB
	pLayerCfg.Alpha0		  = 0xFF;                           // 配置 layer0 的恒定透明度，最终写入 LTDC_LxCACR 寄存器 
                                                                //	需要注意的是，这个参数是直接配置整个 layer0 的透明度，这里设置为255即不透明 
    // 配置 layer0 的初始默认颜色，包括A,R,G,B 的值 ，最终写入 LTDC_LxDCCR 寄存器
	pLayerCfg.Alpha			  = 0xFF;
	pLayerCfg.Backcolor.Red	  = 0x0; 
	pLayerCfg.Backcolor.Green = 0x0;
    pLayerCfg.Backcolor.Blue  = 0x0;

    // 设置 layer0 的层混合系数，最终写入 LTDC_LxBFCR 寄存器 
    // 该参数用于设置 layer0 和 底层背景 之间的颜色混合系数，计算公式为 ：
    // 混合后的颜色 =  BF1 * layer0的颜色 + BF2 * 底层背景的颜色
    // 如果 layer0 使用了透明色，则必须配置成 LTDC_BLENDING_FACTOR1_PAxCA 和 LTDC_BLENDING_FACTOR2_PAxCA，否则ARGB中的A通道不起作用
    //	更多的介绍可以查阅 参考手册关于 LTDC_LxBFCR 寄存器的介绍
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;       // 混合系数1
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;       // 混合系数2

	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_LTDC_ProgramLineEvent(&hltdc, LCD_HEIGHT);    // 设置行中断，第272行
}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef* ltdcHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//避免闪屏，先关闭背光和LCD_DISPLAY
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
	
	if(ltdcHandle->Instance==LTDC)
	{
		__HAL_RCC_LTDC_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		/**LTDC GPIO Configuration
		PC4     ------> LTDC_R7         PD3      ------> LTDC_G7            PD2      ------> LTDC_B7
		PB1     ------> LTDC_R6         PC7      ------> LTDC_G6            PA15     ------> LTDC_B6      
		PC0     ------> LTDC_R5         PC1      ------> LTDC_G5            PA3      ------> LTDC_B5
        PA5     ------> LTDC_R4         PB10     ------> LTDC_G4            PE12     ------> LTDC_B4
		PB0     ------> LTDC_R3         PE11     ------> LTDC_G3            PD10     ------> LTDC_B3
                                    	PA6      ------> LTDC_G2
        
		PA4     ------> LTDC_VSYNC
		PC5     ------> LTDC_DE
        PE14    ------> LTDC_CLK
		PC6     ------> LTDC_HSYNC*/
		
		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
							  |GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
							  |GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_14;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		/* LCD_BL PE3*/
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET);

		/*LCD_DISP PA7 */
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
//		HAL_NVIC_SetPriority(LTDC_IRQn, 6, 0);
//		HAL_NVIC_EnableIRQ(LTDC_IRQn);
	}
}

void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* ltdcHandle)
{
	if(ltdcHandle->Instance==LTDC)
	{
		/* Peripheral clock disable */
		__HAL_RCC_LTDC_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5
								|GPIO_PIN_6|GPIO_PIN_7);

		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
								|GPIO_PIN_15);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10);

		HAL_GPIO_DeInit(GPIOE, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_14);

		HAL_GPIO_DeInit(GPIOD, GPIO_PIN_10|GPIO_PIN_2|GPIO_PIN_3);

		HAL_NVIC_DisableIRQ(LTDC_IRQn);
	}
}
