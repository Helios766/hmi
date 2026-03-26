#include "main.h"
#include "bsp_timer.h"
#include "bsp_dma.h"
#include "bsp_crc.h"
#include "bsp_mpu.h"

#include "OSPI_W25Qxx.h"

#include "lcd_base.h"
#include "debug_led.h"
#include <string.h>
#include <stdint.h>

void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MPU_Memory_Protect_Init(void);


int main(void)
{
	MPU_Memory_Protect_Init();
	HAL_Init();
	SystemClock_Config();
	PeriphCommonClock_Config();
	
	debug_led_init();
	HAL_TIM_Base_Start_IT(&htim6); 
	MX_CRC_Init();
	lcd_init();
	HAL_Delay(2000);
	
	lcd_rect_fill(0,0,240,136,COLOR_RED);
	lcd_rect_fill(240,136,240,136,COLOR_GREEN);
	
	debug_led_set(200,200);
	while (1)
	{
	}
}



void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	
	RCC->CKGAENR = 0xE003FFFF;
	
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
		
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 3;
	RCC_OscInitStruct.PLL.PLLN = 70;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 12;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
								  |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_RCC_EnableCSS();
}

void PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_SPI1
												|RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLL3.PLL3M = 24;
	PeriphClkInitStruct.PLL3.PLL3N = 144;
	PeriphClkInitStruct.PLL3.PLL3P = 2;
	PeriphClkInitStruct.PLL3.PLL3Q = 3;
	PeriphClkInitStruct.PLL3.PLL3R = 16;
	PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
	PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
	PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
	PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL3;
	PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}

void MPU_Memory_Protect_Init(void)
{
    MPU_MemeoryRemap(0x20000000, MPU_REGION_NUMBER0, MPU_REGION_SIZE_128KB, MPU_REGION_FULL_ACCESS, 0, 1, 1);   //保护整个DTCM,共128K字节,允许指令访问,禁止共用,允许cache,允许缓冲
    MPU_MemeoryRemap(0x24000000, MPU_REGION_NUMBER0, MPU_REGION_SIZE_1MB, MPU_REGION_FULL_ACCESS, 0, 1, 1);     //保护整个AXI SRAM,共512K字节,允许指令访问,禁止共用,允许cache,允许缓冲
//    MPU_MemeoryRemap(0x90000000,MPU_REGION_NUMBER0,MPU_REGION_SIZE_16MB,MPU_REGION_FULL_ACCESS,0,1,1); //配置OCTOSPI1映射区为16MB,允许指令访问,禁止共用,允许cache,允许缓冲
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM17)
	{
		HAL_IncTick();
	}
	if(htim->Instance == TIM6 )
	{
		
	}
}

void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{

	}
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
