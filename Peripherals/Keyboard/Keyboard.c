#include "Keyboard.h"
#include "bsp_i2c.h"
#include "bsp_timer.h"
#include <stdbool.h>


// 外部I2C3句柄
extern I2C_HandleTypeDef hi2c3;

// 静态变量定义
static TCA8418_Device KeyEvent_FIFO;  // 键值缓冲区结构体
static KeyMsg_t Key_Event;

// 私有函数声明

static void Keyboard_exit_gpio_init(void);
static inline HAL_StatusTypeDef TCA8418_ReadRegister(uint8_t reg, uint8_t *data);
static inline HAL_StatusTypeDef TCA8418_WriteRegister(uint8_t reg, uint8_t data);
static inline HAL_StatusTypeDef TCA8418_Keyboard_Configure(void);
static inline HAL_StatusTypeDef TCA8418_Interrupt_Config(void);
static inline HAL_StatusTypeDef TCA8418_BurstRead(uint8_t start_reg, uint8_t *data, uint8_t len);
static void TCA8418_KeyEvent_FIFO_Init(void);
static bool TCA8418_KeyEvent_FIFO_Push(uint8_t *keyevent,uint8_t size);
static bool TCA8418_KeyEvent_FIFO_Pop(uint8_t *keyevent,uint8_t size);



// ==================== 私有函数 ====================//

void Keyboard_exit_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /*Configure GPIO pin : PE1 */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/**
  * @brief  读取寄存器
  * @param  reg: 寄存器地址
  * @param  data: 数据指针
  * @retval 成功返回true，失败返回false
  */
static inline HAL_StatusTypeDef TCA8418_ReadRegister(uint8_t reg, uint8_t *data)
{
    // 读取数据  
    return HAL_I2C_Mem_Read(&hi2c3, TCA8418_I2C_ADDR, reg ,I2C_MEMADD_SIZE_8BIT ,data ,1 , 100);
}

/**
  * @brief  连续读取多个寄存器
  * @param  start_reg: 起始寄存器地址
  * @param  data: 数据缓冲区
  * @param  len: 读取长度
  * @retval 成功返回true，失败返回false
  */
static inline HAL_StatusTypeDef TCA8418_BurstRead(uint8_t start_reg, uint8_t *data, uint8_t len)
{
    return HAL_I2C_Mem_Read(&hi2c3, TCA8418_I2C_ADDR, start_reg ,I2C_MEMADD_SIZE_8BIT ,data ,len , 100);
}



/**
  * @brief  写入寄存器
  * @param  reg: 寄存器地址
  * @param  data: 数据
  * @retval 成功返回true，失败返回false
  */
static inline HAL_StatusTypeDef TCA8418_WriteRegister(uint8_t reg, uint8_t data)
{
    // HAL_StatusTypeDef ret = HAL_OK;
    // uint8_t buffer[2] = {reg, data};
    
    // ret = HAL_I2C_Master_Transmit(&hi2c3, TCA8418_I2C_ADDR, buffer, 2, HAL_MAX_DELAY);
    // if(ret != HAL_OK) return ret;
    
    return HAL_I2C_Mem_Write(&hi2c3, TCA8418_I2C_ADDR, reg ,I2C_MEMADD_SIZE_8BIT ,&data ,1 , 100);
}


/**
 * @brief Configure TCA8418 for keypad and GPIO operation
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 * @note This function configures the TCA8418 for keypad operation with pins ROW0 and COL6:0.(7 keys in total)   
 *       It also configures other unused pins as GPIO inputs with pull-up enabled without interrupts.
 */
static inline HAL_StatusTypeDef TCA8418_Keyboard_Configure(void)
{
    HAL_StatusTypeDef ret = HAL_OK;
	uint8_t data=0x00;

    // 配置行（R0-R6作为键盘行，R7不使用）
    ret = TCA8418_WriteRegister(TCA8418_REG_KP_GPIO1, 0x7F);  // 01111111b
    if (ret != HAL_OK) return ret;

    // 配置列（C0-C3作为键盘列，C4作为LED输出，C5-C7不使用）
    ret = TCA8418_WriteRegister(TCA8418_REG_KP_GPIO2, 0x0F);  // 00001111b
    if (ret != HAL_OK) return ret;

    // 配置列（C0-C3作为键盘列，C4作为LED输出，C5-C7不使用）
    ret = TCA8418_WriteRegister(TCA8418_REG_KP_GPIO3, 0x00);  // 00000000b
    if (ret != HAL_OK) return ret;
	
	
    // 设置C4为输出模式，初始为低电平（LED灭）	
	ret = TCA8418_ReadRegister(TCA8418_REG_GPIO_DIR2,&data);
    if(ret != HAL_OK) return ret;
	data |= (1 << LED_COL);
    ret = TCA8418_WriteRegister(TCA8418_REG_GPIO_DIR2, data);  



    ret = TCA8418_LED_Control(0);

	return ret;
}

/**
 * @brief Enable TCA8418 key events interrupt
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */ 
static inline HAL_StatusTypeDef TCA8418_Interrupt_Config(void)
{
    HAL_StatusTypeDef ret;
    uint8_t data = 0x01; // KE_IEN[0] = 1
    ret = TCA8418_WriteRegister(TCA8418_REG_GPIO_INT_EN1, 0x7F);  // R0-R6中断
    if (ret != HAL_OK) return ret;
    ret = TCA8418_WriteRegister(TCA8418_REG_GPIO_INT_EN2, 0x0F);  // C0-C3中断
    if (ret != HAL_OK) return ret;
	
	// 清除旧中断
    ret = TCA8418_ReadRegister(TCA8418_REG_INT_STAT, &data);
    if (ret != HAL_OK) return ret;
	
    return HAL_OK;
}

/**
  * @brief  初始化缓冲区
  */
static void TCA8418_KeyEvent_FIFO_Init(void)
{
    KeyEvent_FIFO.buffer_head = 0;
    KeyEvent_FIFO.buffer_tail = 0;
	KeyEvent_FIFO.buffer_count = 0;
	for(uint8_t i = 0;i < KEY_BUFFER_SIZE; i++)
	{
		KeyEvent_FIFO.key_buffer[i] = 0;
	}
}

/**
  * @brief  压入按键事件到缓冲区
  * @param  keyevent: 按键事件数据指针
  * @param  size: 事件数量（字节数）
  * @retval 成功返回true，缓冲区满返回false
  */
static bool TCA8418_KeyEvent_FIFO_Push(uint8_t *keyevent, uint8_t size)
{
    if (KeyEvent_FIFO.buffer_count + size > KEY_BUFFER_SIZE)
        return false;

    for (uint8_t i = 0; i < size; i++)
    {
        // 先写入当前 tail 指向的位置
        KeyEvent_FIFO.key_buffer[KeyEvent_FIFO.buffer_tail] = keyevent[i];
        // 然后 tail 向后移动一位（取模）
        KeyEvent_FIFO.buffer_tail = (KeyEvent_FIFO.buffer_tail + 1) % KEY_BUFFER_SIZE;
        KeyEvent_FIFO.buffer_count++;
    }
    return true;
}



/**
  * @brief  从缓冲区弹出按键事件
  * @param  keyevent: 按键事件指针
  * @retval 成功返回true，缓冲区空返回false
  */
static bool TCA8418_KeyEvent_FIFO_Pop(uint8_t *keyevent, uint8_t size)
{
    if (KeyEvent_FIFO.buffer_count < size)
        return false;

    for (uint8_t i = 0; i < size; i++)
    {
        // 从当前 head 指向的位置读取
        keyevent[i] = KeyEvent_FIFO.key_buffer[KeyEvent_FIFO.buffer_head];
        // 然后 head 向后移动一位（取模）
        KeyEvent_FIFO.buffer_head = (KeyEvent_FIFO.buffer_head + 1) % KEY_BUFFER_SIZE;
        KeyEvent_FIFO.buffer_count--;
    }
    return true;
}

// ==================== 公共函数 ==================== //
HAL_StatusTypeDef Keyboard_Init(void)
{
    HAL_StatusTypeDef ret = HAL_OK;

    TCA8418_KeyEvent_FIFO_Init();
    Keyboard_exit_gpio_init();
    MX_I2C3_Init();
    MX_TIM4_Init();
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

    // 复位寄存器
    ret = TCA8418_WriteRegister(TCA8418_REG_CFG, 0x00);
    if(ret != HAL_OK) return ret;

    HAL_Delay(10);

    // 配置TCA8418
    ret = TCA8418_WriteRegister(TCA8418_REG_CFG, CFG_AI | CFG_INT_CFG | CFG_KE_IEN);// 自动递增，中断有效
    if(ret != HAL_OK) return ret;

	
    // 配置键盘为7行4列，C4为LED输出
    ret = TCA8418_Keyboard_Configure();
    if(ret != HAL_OK) return ret;

    ret = TCA8418_Interrupt_Config();
    if(ret != HAL_OK) return ret;

	return ret;
}

/**
  * @brief  设置LED状态
  * @param  state: 状态 (0=关, 1=开)
  * @retval 成功返回true，失败返回false
  */
HAL_StatusTypeDef TCA8418_LED_Control(GPIO_PinState state)
{
    uint8_t gpio_data;
    HAL_StatusTypeDef ret = HAL_OK;
    
    // 读取当前的GPIO数据
	ret = TCA8418_ReadRegister(TCA8418_REG_GPIO_DAT_OUT2, &gpio_data);
    if (ret != HAL_OK) return ret;

    // 设置C4位
    if (state == GPIO_PIN_SET) 
    {
        gpio_data |= (1 << LED_COL);   // 置位
    } 
    else 
    {
        gpio_data &= ~(1 << LED_COL);  // 清除
    }
    
    // 写回GPIO数据
    ret = TCA8418_WriteRegister(TCA8418_REG_GPIO_DAT_OUT2, gpio_data);
    if (ret != HAL_OK) return ret;
	
    return ret;
}

uint8_t keyevent_count;
uint8_t keyevent_temp[TCA8418_KEY_EVENT_FIFO_SIZE];
HAL_StatusTypeDef TCA8418_DMA_ReadKeyEvent(void)
{
    HAL_StatusTypeDef ret = HAL_OK;

    ret = TCA8418_ReadRegister(TCA8418_REG_KEY_LCK_EC, &keyevent_count);      //先读取按键事件数量
	if(ret != HAL_OK) return ret;
	
	if(keyevent_count > 0)
	{
		ret = HAL_I2C_Mem_Read_DMA(&hi2c3, 
                           TCA8418_I2C_ADDR,           	 // 设备地址（已左移1位）
                           TCA8418_REG_KEY_EVENT_A,    	 // 要读取的起始寄存器地址
                           I2C_MEMADD_SIZE_8BIT,       	 // 寄存器地址为8位
                           keyevent_temp,               // 接收缓冲区
                           keyevent_count);                // 按需求读取按键事件
		if(ret != HAL_OK) return ret;
	}
    return HAL_OK;
}


void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == I2C3)
	{
        TCA8418_KeyEvent_FIFO_Push(keyevent_temp,keyevent_count);

		// 这里可以根据实际需求返回一个按键事件，或者将事件存储在全局变量中供其他函数使用
	}
}

/*************************************************按键处理实现**********************************************************/
static uint32_t key_tick = 0;      //长按计时

bool TCA8418_Get_KeyEvent(KeyMsg_t *msg)
{
	uint8_t event_byte;
    static KeyMsg_t key_sm;

    if(TCA8418_KeyEvent_FIFO_Pop(&event_byte, 1))
    {
        uint8_t pressed = (event_byte >> 7) & 0x01;
        uint8_t keycode = event_byte & 0x7F;
        if(pressed && key_sm.key_code == 0xFF)   //按键按下
        {
            key_sm.key_code = keycode;
            key_tick = 1;
            return false;
        }
        else if(!pressed && key_sm.key_code != 0xFF && keycode == key_sm.key_code) 
        {
            if(key_tick >= LONG_PRESS_TIME_MS)
            {
                key_sm.event = KEY_EVENT_LONG_PRESS;
            }
            else 
            {
                key_sm.event = KEY_EVENT_PRESS;
            }
            *msg = key_sm;
            key_sm.key_code = 0xff;
            key_sm.key_code = KEY_EVENT_RELEASE;
            key_tick = 0;
            return true;
        }
        else 
        {
            return false;
        }
    }
    else 
    {
        return false;
    }
}

// 1ms 定时器回调（需在定时器中断中调用）
void TCA8418_Get_KeyTick(void)
{
	if(key_tick > 0)
	{
		key_tick++;
	}
}
