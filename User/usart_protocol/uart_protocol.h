#ifndef __UART_PROTOCOL_H_
#define __UART_PROTOCOL_H_

#include "main.h"
#include <stdint.h>


#define PROTOCOL_MAX_DATA_LEN 1024  //协议最大帧长

#define PROTOCOL_MAX_BYTE 9   //除数据外的最大协议字节数


#pragma pack(push, 1)
typedef struct{
    uint16_t header;        //帧头
    uint8_t cmd;            //指令码
    uint16_t seq;           //帧序列号uint
    uint16_t length;        //数据长度
    uint8_t protocol_data[PROTOCOL_MAX_DATA_LEN];
    uint16_t crc16;
}protocol_frame_t;
#pragma pack(pop)

// 协议状态
typedef enum {
    PROTOCOL_IDLE,
    PROTOCOL_RECEIVING,
    PROTOCOL_RECEIVED,
    PROTOCOL_PROCESSING,
    PROTOCOL_SENDING,
    PROTOCOL_WAITING_RESPONSE,
    PROTOCOL_ERROR
} protocol_state_t;

typedef struct{
    protocol_state_t state;
    uint32_t last_activity_time; 
} protocol_manager_t;


// extern protocol_cb_t protocol_cb;

void uart_protocol_init(void);
uint8_t UART_SendData_DMA(uint8_t *data, uint16_t len);
#endif
