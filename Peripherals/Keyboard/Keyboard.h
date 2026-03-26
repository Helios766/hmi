#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "main.h"


// TCA8418 I2C地址 (A1=0, A0=0)
#define TCA8418_I2C_ADDR          0x34 << 1  // 7位地址左移1位

// TCA8418 内部FIFO大小
#define TCA8418_KEY_EVENT_FIFO_SIZE 10
// 寄存器地址 (基于TI官方手册)
#define TCA8418_REG_CFG            	0x01
#define TCA8418_REG_INT_STAT       	0x02
#define TCA8418_REG_KEY_LCK_EC     	0x03
#define TCA8418_REG_KEY_EVENT_A    	0x04
#define TCA8418_REG_KEY_EVENT_B    	0x05
#define TCA8418_REG_KEY_EVENT_C    	0x06
#define TCA8418_REG_KEY_EVENT_D    	0x07
#define TCA8418_REG_KEY_EVENT_E    	0x08
#define TCA8418_REG_KEY_EVENT_F    	0x09
#define TCA8418_REG_KEY_EVENT_G    	0x0A
#define TCA8418_REG_KEY_EVENT_H    	0x0B
#define TCA8418_REG_KEY_EVENT_I    	0x0C
#define TCA8418_REG_KEY_EVENT_J    	0x0D
#define TCA8418_REG_KP_LCK_TIMER   	0x0E
#define TCA8418_REG_UNLOCK1        	0x0F
#define TCA8418_REG_UNLOCK2        	0x10
#define TCA8418_REG_GPIO_INT_STAT1 	0x11
#define TCA8418_REG_GPIO_INT_STAT2 	0x12
#define TCA8418_REG_GPIO_INT_STAT3 	0x13
#define TCA8418_REG_GPIO_DAT_STAT1 	0x14
#define TCA8418_REG_GPIO_DAT_STAT2 	0x15
#define TCA8418_REG_GPIO_DAT_STAT3 	0x16
#define TCA8418_REG_GPIO_DAT_OUT1  	0x17
#define TCA8418_REG_GPIO_DAT_OUT2  	0x18
#define TCA8418_REG_GPIO_DAT_OUT3  	0x19
#define TCA8418_REG_GPIO_INT_EN1   	0x1A
#define TCA8418_REG_GPIO_INT_EN2   	0x1B
#define TCA8418_REG_GPIO_INT_EN3   	0x1C
#define TCA8418_REG_KP_GPIO1       	0x1D   // R0-R7
#define TCA8418_REG_KP_GPIO2       	0x1E   // C0-C7
#define TCA8418_REG_KP_GPIO3       	0x1F   // C8-C9
#define TCA8418_REG_GPI_EM1         0x20
#define TCA8418_REG_GPI_EM2         0x21
#define TCA8418_REG_GPI_EM3         0x22
#define TCA8418_REG_GPIO_DIR1       0x23
#define TCA8418_REG_GPIO_DIR2       0x24
#define TCA8418_REG_GPIO_DIR3       0x25
#define TCA8418_REG_GPIO_INT_LVL1   0x26
#define TCA8418_REG_GPIO_INT_LVL2   0x27
#define TCA8418_REG_GPIO_INT_LVL3   0x28
#define TCA8418_REG_DEBOUNCE_DIS1   0x29
#define TCA8418_REG_DEBOUNCE_DIS2   0x2A
#define TCA8418_REG_DEBOUNCE_DIS3   0x2B
#define TCA8418_REG_GPIO_PULL1      0x2C
#define TCA8418_REG_GPIO_PULL2      0x2D
#define TCA8418_REG_GPIO_PULL3      0x2E

// CFG寄存器位定义
#define CFG_AI               0x80  // 自动递增
#define CFG_GPI_EVENT_MODE   0x40  // GPI事件模式（当键盘锁定时）
#define CFG_OVR_FLOW_M       0x20  // 溢出模式
#define CFG_INT_CFG          0x10  // 中断配置（50μs脉冲）
#define CFG_OVR_FLOW_IEN     0x08  // 溢出中断使能
#define CFG_K_LCK_IEN        0x04  // 键盘锁中断使能
#define CFG_GPI_IEN          0x02  // GPI中断使能
#define CFG_KE_IEN           0x01  // 按键事件中断使能

// 键盘矩阵配置
#define ROW_NUM              7     // 行数 (R0-R6)
#define COL_NUM              4     // 列数 (C0-C3)
#define LED_COL              4     // LED使用的列 (C4)
#define KEY_BUFFER_SIZE      40    // 按键缓冲区大小


#define LONG_PRESS_TIME_MS      2000      // 长按阈值（毫秒）
#define MAX_KEY_NUM             128      // 最大键码数（7位键码范围0-127）
#define KEY_MSG_QUEUE_SIZE      10       // 消息队列大小

// TCA8418 键值缓冲结构体
typedef struct {
    uint8_t key_buffer[KEY_BUFFER_SIZE];  // 按键缓冲区
    uint8_t buffer_head;          // 缓冲区头指针
    uint8_t buffer_tail;          // 缓冲区尾指针
    uint8_t buffer_count;         // 缓冲区内的事件数量
} TCA8418_Device;

// 按键事件类型
typedef enum {
    KEY_EVENT_RELEASE = 0,      // 释放（短按）
    KEY_EVENT_PRESS,            // 按下
    KEY_EVENT_LONG_PRESS,       // 长按（达到阈值时触发）
} KeyEventType_t;

// 按键事件结构体
typedef struct {
    uint8_t key_code;           // 键值
    KeyEventType_t event;         // 按下/释放
}KeyMsg_t;


HAL_StatusTypeDef Keyboard_Init(void);
bool TCA8418_GetKeyEvent(KeyMsg_t *msg);
HAL_StatusTypeDef TCA8418_DMA_ReadKeyEvent(void);
HAL_StatusTypeDef TCA8418_LED_Control(GPIO_PinState state);
void TCA8418_Get_KeyTick(void);


#endif 

