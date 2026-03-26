#include "uart_protocol.h"
#include "uart_data_queue.h"
#include "bsp_crc.h"

#include <string.h>
#include <stdint.h>

#include "lcd_console.h"

protocol_manager_t protocol_manager;

void uart_protocol_init(void)
{
	uart_buffer_queue_init();
}

// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
// 	if (huart == &huart1)
//     {
//         // 确保 Size 至少包含头部（7字节）
//         if ((Size < 7)||Size >= (PROTOCOL_MAX_DATA_LEN - PROTOCOL_MAX_BYTE)) return;

//         uint8_t *buf = (uint8_t*)uart_rx_buf;
        
//         // 解析头部（根据协议字节序调整）
//         uint16_t header = buf[0] | (buf[1] << 8);   // 小端
//         uint8_t  cmd    = buf[2];
//         uint16_t seq    = buf[3] | (buf[4] << 8);   // 小端
//         uint16_t length = buf[5] | (buf[6] << 8);   // 小端

//         if (header != 0xAA55) return;  // 帧头错误

//         // 计算 CRC 指针
//         uint8_t *crc_ptr = buf + 7 + length;
//         uint16_t crc_received = (crc_ptr[0]<<8) | crc_ptr[1]; 

//         // 计算预期 CRC：校验前 (7 + length) 字节
//         uint16_t crc_calculated = hardware_crc16_calculate(buf, 7 + length);

//         if (crc_calculated == crc_received)
//         {
//             protocol_manager.state = PROTOCOL_RECEIVED;
//             protocol_manager.last_activity_time = HAL_GetTick();
			
// 			console_printf("\033[30;32mData parsing successful!\033[30;37m\n");
// 			console_printf("the source data is: \033[30;32m");
// 			for(uint16_t i=0;i<Size;i++)
// 			{
// 				console_printf("%02X ",uart_rx_buf[i]);
// 			}
// 			console_printf("\033[30;37m\n");
// 			console_printf("CRC check value is: \033[30;32m0x%04X\033[30;37m\n",crc_calculated);
//         }
//     }
// }

// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
// 	{
// 		memset(uart_rx_buf,0,rx_len);
//     }
// }

