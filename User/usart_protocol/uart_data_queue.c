#include "uart_data_queue.h"
#include "bsp_usart.h"
#include "bsp_dma.h"

#include <stdint.h>
#include <string.h>

//队列缓冲区的内存池
uint8_t uart_rx_buf[DMA_UART_BUFFER_SIZE] __attribute__((used,section(".USART_PROTOCOL_RAM"))) = {0};			//H7 DMA 访问空间有要求, 无法访问DTCM 0x2000000区域的RAM
uint8_t queue_ring_rx_buf[QUEUE_RING_BUFFER_SIZE] = {0};	//H7 DMA 访问空间有要求, 无法访问DTCM 0x2000000区域的RAM

static ring_buffer_t uart_rx_queue;

void uart_buffer_queue_init(void)
{
	MX_DMA_Init();
	MX_USART1_UART_Init();
	// 开启UART1 DMA+空闲中断接收，指定临时缓存和最大接收字节数
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1,uart_rx_buf,DMA_UART_BUFFER_SIZE);
	
	ring_buffer_init(&uart_rx_queue,queue_ring_rx_buf,QUEUE_RING_BUFFER_SIZE);
}

/**
 * @brief 初始化环形缓冲区
 * @param rb  缓冲区对象指针
 * @param temp_buf 外部提供的缓冲区（需至少 size 字节）
 * @param size 缓冲区大小（字节数，必须 >=2 且为 2 的幂可提高取模效率）
 */
void ring_buffer_init(ring_buffer_t *cb,uint8_t *temp_buf,uint32_t buff_size)
{
	cb->buffer = temp_buf;
	cb->size = buff_size;
	cb->head = 0;
	cb->tail = 0;
	cb->last_dma_pos = 0;
}

/**
 * @brief 判断缓冲区是否为空
 * @param rb 缓冲区对象指针
 * @return true 空，false 非空
 */
bool ring_buffer_is_empty(ring_buffer_t *rb)
{
    return rb->head == rb->tail;
}

/**
 * @brief 判断缓冲区是否为满
 * @param rb 缓冲区对象指针
 * @return true 满，false 未满
 */
bool ring_buffer_is_full(ring_buffer_t *rb)
{
    // 下一个写指针追上读指针即为满
    return ((rb->head + 1) % rb->size) == rb->tail;
}

/**
 * @brief 获取缓冲区中当前可读数据量（字节数）
 * @param rb 缓冲区对象指针
 * @return 可读字节数
 */
uint32_t ring_buffer_available_data(ring_buffer_t *rb)
{
    return (rb->head - rb->tail) % rb->size;
}

/**
 * @brief 获取缓冲区中剩余可写空间（字节数）
 * @param rb 缓冲区对象指针
 * @return 剩余空间字节数
 */
uint32_t ring_buffer_available_space(ring_buffer_t *rb)
{
    // 保留一个空位，所以实际可用空间为 size - 1 - 已有数据
    return rb->size - 1 - ring_buffer_available_data(rb);
}

/**
 * @brief 向缓冲区写入单个字节
 * @param rb   缓冲区对象指针
 * @param data 要写入的字节
 * @return true 写入成功，false 缓冲区满
 */
bool ring_buffer_write_byte(ring_buffer_t *rb, uint8_t data)
{
    if (ring_buffer_is_full(rb)) 
    {
        return false;
    }
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->size;
    return true;
}

/**
 * @brief 从缓冲区读取单个字节
 * @param rb   缓冲区对象指针
 * @param data 存储读取字节的变量指针
 * @return true 读取成功，false 缓冲区空
 */
bool ring_buffer_read_byte(ring_buffer_t *rb, uint8_t *data)
{
    if (ring_buffer_is_empty(rb)) 
    {
        return false;
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    return true;
}

/**
 * @brief 向缓冲区写入多个字节（可能部分成功）
 * @param rb   缓冲区对象指针
 * @param data 待写入的数据指针
 * @param len  请求写入的字节数
 * @return 实际写入的字节数（<= len）
 */
uint32_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, uint32_t len)
{
    uint32_t space = ring_buffer_available_space(rb);
    uint32_t to_write = (len < space) ? len : space;   // 最多写可用空间
    uint32_t first_chunk = rb->size - rb->head;        // 从 head 到缓冲区末尾的连续空间

    if (to_write <= first_chunk) 
    {
        // 一次拷贝即可完成
        memcpy(&rb->buffer[rb->head], data, to_write);
        rb->head += to_write;
        if (rb->head >= rb->size) 
        {
            rb->head -= rb->size;   // 因为 to_write <= first_chunk，此处仅当 to_write == first_chunk 时 head 变为 0
        }
    } 
    else 
    {
        // 分两段拷贝：先填满尾部，再从开头继续
        memcpy(&rb->buffer[rb->head], data, first_chunk);
        memcpy(rb->buffer, data + first_chunk, to_write - first_chunk);
        rb->head = to_write - first_chunk;
    }
    return to_write;
}

/**
 * @brief 从缓冲区读取多个字节（可能部分成功）
 * @param rb   缓冲区对象指针
 * @param data 存储读取数据的指针
 * @param len  请求读取的字节数
 * @return 实际读取的字节数（<= len）
 */
uint32_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, uint32_t len)
{
    uint32_t available = ring_buffer_available_data(rb);
    uint32_t to_read = (len < available) ? len : available;
    uint32_t first_chunk = rb->size - rb->tail;        // 从 tail 到缓冲区末尾的连续数据

    if (to_read <= first_chunk) 
    {
        memcpy(data, &rb->buffer[rb->tail], to_read);
        rb->tail += to_read;
        if (rb->tail >= rb->size) 
        {
            rb->tail -= rb->size;
        }
    }
    else 
    {
        memcpy(data, &rb->buffer[rb->tail], first_chunk);
        memcpy(data + first_chunk, rb->buffer, to_read - first_chunk);
        rb->tail = to_read - first_chunk;
    }
    return to_read;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
		uint32_t current_pos = DMA_UART_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx); // 当前DMA写入位置
        uint32_t new_data_len ;
        uint32_t witten;

        if(current_pos >= uart_rx_queue.last_dma_pos)
        {
            new_data_len = current_pos - uart_rx_queue.last_dma_pos;
            // 数据连续，直接写入
            witten = ring_buffer_write(&uart_rx_queue, &uart_rx_buf[uart_rx_queue.last_dma_pos], new_data_len);
            if(witten < new_data_len)
            {
                //环形缓冲区满，无法写入全部数据，丢弃剩余部分
            }
        }
        else
        {
            uint32_t first_part = DMA_UART_BUFFER_SIZE - uart_rx_queue.last_dma_pos; // 从上次位置到缓冲区末尾的数据长度
            // 先写入第一部分
            witten = ring_buffer_write(&uart_rx_queue, &uart_rx_buf[uart_rx_queue.last_dma_pos], first_part);
            if(witten < first_part)
            {
                //环形缓冲区满，无法写入全部数据，丢弃剩余部分

            }

            witten = ring_buffer_write(&uart_rx_queue, uart_rx_buf, current_pos); // 再写入从缓冲区开头到当前DMA位置的数据
            if(witten < current_pos)
            {
                //环形缓冲区满，无法写入全部数据，丢弃剩余部分
            }
        }
        uart_rx_queue.last_dma_pos = current_pos; // 更新DMA位置以继续接收新数据
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		//memset(uart_rx_buf,0,rx_len);
	}
}

