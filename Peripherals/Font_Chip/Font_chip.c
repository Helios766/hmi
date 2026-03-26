#include "Font_chip.h"
#include "bsp_spi.h"

#include <stdint.h>

//#include "bsp_dma.h"

// uint8_t	SPI_TX_Buff[1];     // 要发送的指令
// uint8_t	SPI_RX_Buff[4];		// 存储SPI读到的数据

// uint8_t Font_chip_cheak_Data[]={
//     0x63, 0x15, 0x00, 0x00, 0x06, 0x00, 0x81, 0x15,
//     0x00, 0x00, 0x06, 0x00, 0x9E, 0x15, 0x00, 0x00
// };

unsigned char gt_read_data(unsigned char *sendbuf,unsigned char sendlen,unsigned char *receivebuf,unsigned int receivelen)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	
	status = SPI_Transmit_Receive(&hspi1,sendbuf,receivebuf,sendlen,receivelen);
	if(status != HAL_OK)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


char Font_Chip_Init(void)
{
	char status = -1;
    MX_SPI1_Init();
	status = GT_Font_Init();
    return status;
}

void show_char(uint16_t x,uint16_t y,uint8_t text,uint16_t fg_color,uint16_t bg_color,uint16_t font_size)
{
//    uint16_t i,j;
//    uint8_t count = 0;
//    for(i = 0; i < 8; i++)
//    {
//        if(((text >> (7 - i)) & 0x01) == 0x01)
//        {
//            for(count = 0;count < font_size; count++)
//            {
//                for(j = 0;j < font_size;j++)
//                {
//                    lcd_draw_point(x + font_size*i + count,y+j,fg_color);
//                }
//            }
//        }
//        else 
//        {
//            for(count = 0;count < font_size; count++)
//            {
//                for(j = 0;j < font_size;j++)
//                {
//                    lcd_draw_point(x + font_size*i + count,y+j,bg_color);
//                }
//            }
//        }
//    }
}


