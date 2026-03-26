#ifndef _UART_DATA_QUEUE_H__
#define _UART_DATA_QUEUE_H__

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define DMA_UART_BUFFER_SIZE 			1024        //收发缓冲区大小
#define QUEUE_RING_BUFFER_SIZE		2048

/**
 * @brief 环形缓冲区结构体
 */
typedef struct {
    uint8_t *buffer;    // 指向外部提供的缓冲区
    uint32_t size;      // 缓冲区总大小（字节数）
    uint32_t head;      // 写指针（指向下一个要写入的位置）
    uint32_t tail;      // 读指针（指向下一个要读取的位置）
    uint32_t last_dma_pos; // 上次 DMA 写入位置（剩余计数或写入位置）
} ring_buffer_t;


void uart_buffer_queue_init(void);
void ring_buffer_init(ring_buffer_t *cb,uint8_t *temp_buf,uint32_t buff_size);
bool ring_buffer_is_empty(ring_buffer_t *rb);
bool ring_buffer_is_full(ring_buffer_t *rb);
uint32_t ring_buffer_available_data(ring_buffer_t *rb);
uint32_t ring_buffer_available_space(ring_buffer_t *rb);
bool ring_buffer_write_byte(ring_buffer_t *rb, uint8_t data);
bool ring_buffer_read_byte(ring_buffer_t *rb, uint8_t *data);
uint32_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, uint32_t len);
uint32_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, uint32_t len);

#endif
