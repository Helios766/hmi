#ifndef __BSP_LTDC_H__
#define __BSP_LTDC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern LTDC_HandleTypeDef hltdc;

void MX_LTDC_Init(void);

#define HBP  43	// 根据屏幕的手册进行设置
#define VBP  12
#define HSW  1
#define VSW  1
#define HFP  8
#define VFP  8

#define LCD_WIDTH     	480				//	LCD的像素长度
#define LCD_HEIGHT    	272				//	LCD的像素宽度
#define LCD_PIXEL_SIZE  2  				// RGB565 = 2字节
#define LCD_BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT * LCD_PIXEL_SIZE)

//显存地址,使用7B0内部的AXI SRAM作为显存，起始地址0x24000000，大小为1M

#ifndef FRAME_BUFFER0_ADDR
	#define FRAME_BUFFER0_ADDR   	0x24000000					//	显存的起始地址
#endif

#ifndef FRAME_BUFFER1_ADDR
	#define FRAME_BUFFER1_ADDR   	0x2403FC00					//	显存的起始地址
#endif


#ifdef __cplusplus
}
#endif

#endif /* __LTDC_H__ */

