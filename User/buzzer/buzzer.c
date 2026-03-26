#include "buzzer.h"
#include <stdint.h>
#include <string.h>

BuzzerType_t Beep_msg;

// 蜂鸣器状态结构体
typedef struct
{
    BuzzerNote_t current_note;
    uint8_t volume;
    uint8_t is_playing;
}BuzzerState_t;

// 旋律播放状态结构体
typedef struct
{
	const Melody_t *melody;
	BuzzerPriority_t priority;
	uint8_t index;
	uint32_t note_start_time;
}MelodyState_t;

// 旋律队列项
typedef struct
{
    const Melody_t *melody;
    BuzzerPriority_t priority;
} MelodyQueueItem_t;

// 队列配置
#define BUZZER_QUEUE_SIZE 10  // 队列大小，根据需求调整

// 私有变量
static BuzzerState_t buzzer;
static MelodyState_t current_melody;
static MelodyQueueItem_t melody_queue[BUZZER_QUEUE_SIZE];
static uint8_t queue_head = 0;
static uint8_t queue_tail = 0;
static uint8_t queue_count = 0;

// 私有函数声明
static uint8_t Buzzer_EnqueueMelody(const Melody_t *melody, BuzzerPriority_t priority);
static uint8_t Buzzer_DequeueMelody(MelodyQueueItem_t *item);
static uint8_t Buzzer_IsQueueEmpty(void);
static uint8_t Buzzer_IsQueueFull(void);
static void Buzzer_ProcessQueue(void);
static uint8_t Buzzer_ShouldInterrupt(BuzzerPriority_t new_priority);

// 初始化蜂鸣器
void Buzzer_Init(void)
{
// 初始化状态
    buzzer.current_note = BUZZER_NOTE_C4;
    buzzer.volume = 50;
    buzzer.is_playing = 0;
    
    // 初始化旋律状态
    current_melody.melody = NULL;
    current_melody.index = 0;
    current_melody.note_start_time = 0;
    
    // 初始化队列
    memset(melody_queue, 0, sizeof(melody_queue));
    queue_head = 0;
    queue_tail = 0;
    queue_count = 0;
    
    // 硬件初始化
    MX_TIM3_Init();
//	MX_TIM6_Init();
	
	HAL_TIM_Base_Start_IT(&htim6); 
	
    // 启动PWM输出
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    // 初始设置为静音
    Buzzer_SetVolume(100);
}

// 播放旋律（带优先级）
void Buzzer_PlayMelody(const Melody_t *melody, BuzzerPriority_t priority)
{
    if (melody == NULL || melody->note_count == 0)
    {
        return;
    }
    
    // 检查是否需要打断当前播放
    if (Buzzer_ShouldInterrupt(priority))
    {
        // 打断当前播放，直接开始新的旋律
        current_melody.melody = melody;
        current_melody.priority = priority;
        current_melody.index = 0;
        current_melody.note_start_time = HAL_GetTick();
        
        // 播放第一个音符
        if (melody->notes[0].note == BUZZER_SILENCE)
        {
            Buzzer_SetNote(BUZZER_SILENCE);
        }
        else
        {
            Buzzer_SetNote(melody->notes[0].note);
        }
        
        buzzer.is_playing = 1;
    }
    else
    {
        // 不需要打断，加入队列
        Buzzer_EnqueueMelody(melody, priority);
    }
}


// 更新函数（在主循环或定时器中断中调用）
void Buzzer_Update(void)
{
    static uint32_t last_update_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    // 限制更新频率（约100Hz）
    if (current_time - last_update_time < 10)
    {
        return;
    }
    last_update_time = current_time;
    
    // 处理当前旋律
    if (buzzer.is_playing && current_melody.melody != NULL)
    {
        // 检查当前音符是否播放完毕
        uint32_t note_elapsed = current_time - current_melody.note_start_time;
        
        if (note_elapsed >= current_melody.melody->notes[current_melody.index].duration)
        {
            // 切换到下一个音符
            current_melody.index++;
            
            // 检查是否播放完毕
            if (current_melody.index >= current_melody.melody->note_count)
            {
                // 当前旋律播放完毕
                Buzzer_SetNote(BUZZER_SILENCE);
                buzzer.is_playing = 0;
                current_melody.melody = NULL;
                
                // 处理队列中的下一个旋律
                Buzzer_ProcessQueue();
            }
            else
            {
                // 播放下一个音符
                Buzzer_SetNote(current_melody.melody->notes[current_melody.index].note);
                current_melody.note_start_time = current_time;
            }
        }
    }
    else if (!buzzer.is_playing)
    {
        // 没有正在播放的旋律，检查队列
        Buzzer_ProcessQueue();
    }
}

// 判断是否需要打断当前播放
static uint8_t Buzzer_ShouldInterrupt(BuzzerPriority_t new_priority)
{
    // 如果没有正在播放，直接播放
    if (!buzzer.is_playing || current_melody.melody == NULL)
    {
        return 1;
    }
    // 获取当前播放的优先级
    BuzzerPriority_t current_priority = current_melody.priority;
    // 新优先级高于当前优先级打断
    if (new_priority > current_priority)
    {
        return 1;
    }
    // 其他情况不打断
    return 0;
}

// 处理队列中的下一个旋律
static void Buzzer_ProcessQueue(void)
{
	if (!Buzzer_IsQueueEmpty() && !buzzer.is_playing)
    {
        MelodyQueueItem_t item;
        if (Buzzer_DequeueMelody(&item))
        {
            // 开始播放队列中的旋律
            current_melody.melody = item.melody;
            current_melody.priority = item.priority;
            current_melody.index = 0;
            current_melody.note_start_time = HAL_GetTick();
            
            // 播放第一个音符
            if (item.melody->notes[0].note == BUZZER_SILENCE)
            {
                Buzzer_SetNote(BUZZER_SILENCE);
            }
            else
            {
                Buzzer_SetNote(item.melody->notes[0].note);
            }
            
            buzzer.is_playing = 1;
        }
    }
}

// 入队旋律
static uint8_t Buzzer_EnqueueMelody(const Melody_t *melody, BuzzerPriority_t priority)
{
	// 检查队列中是否已经有相同优先级的旋律
    // 避免队列被过多相同优先级的旋律填满
    if (queue_count >= BUZZER_QUEUE_SIZE)
    {
        // 队列已满，检查队列中是否有比新旋律优先级更低的
        BuzzerPriority_t lowest_priority = BUZZER_PRIORITY_EMERGENCY;
        uint8_t lowest_priority_index = queue_head;
        
        for (uint8_t i = 0; i < queue_count; i++)
        {
            uint8_t idx = (queue_head + i) % BUZZER_QUEUE_SIZE;
            if (melody_queue[idx].priority < lowest_priority)
            {
                lowest_priority = melody_queue[idx].priority;
                lowest_priority_index = idx;
            }
        }
        
        // 如果新旋律优先级高于队列中最低优先级的旋律
        if (priority > lowest_priority)
        {
            // 替换最低优先级的旋律
            melody_queue[lowest_priority_index].melody = melody;
            melody_queue[lowest_priority_index].priority = priority;
            return 1;
        }
        
        // 队列已满且新旋律优先级不够高，丢弃新旋律
        return 0;
    }
    
    // 队列未满，正常入队
    melody_queue[queue_tail].melody = melody;
    melody_queue[queue_tail].priority = priority;
    queue_tail = (queue_tail + 1) % BUZZER_QUEUE_SIZE;
    queue_count++;
    
    return 1;
}

// 出队旋律
static uint8_t Buzzer_DequeueMelody(MelodyQueueItem_t *item)
{
    if (Buzzer_IsQueueEmpty())
    {
        return 0;
    }
    
    // 找到队列中优先级最高的项
    uint8_t highest_priority_index = queue_head;
    BuzzerPriority_t highest_priority = melody_queue[queue_head].priority;
    
    for (uint8_t i = 1; i < queue_count; i++)
    {
        uint8_t idx = (queue_head + i) % BUZZER_QUEUE_SIZE;
        if (melody_queue[idx].priority > highest_priority)
        {
            highest_priority = melody_queue[idx].priority;
            highest_priority_index = idx;
        }
    }
    
    // 取出最高优先级的项
    item->melody = melody_queue[highest_priority_index].melody;
    item->priority = melody_queue[highest_priority_index].priority;
    
    // 如果取出的不是队头，需要移动元素
    if (highest_priority_index != queue_head)
    {
        for (uint8_t i = highest_priority_index; i != queue_tail; i = (i + 1) % BUZZER_QUEUE_SIZE)
        {
            uint8_t next = (i + 1) % BUZZER_QUEUE_SIZE;
            melody_queue[i] = melody_queue[next];
        }
    }
    
    // 调整队尾指针
    queue_tail = (queue_tail == 0) ? (BUZZER_QUEUE_SIZE - 1) : (queue_tail - 1);
    queue_count--;
    
    return 1;
}

// 检查队列是否为空
static uint8_t Buzzer_IsQueueEmpty(void)
{
    return queue_count == 0;
}

// 检查队列是否已满
static uint8_t Buzzer_IsQueueFull(void)
{
    return queue_count == BUZZER_QUEUE_SIZE;
}

// 清空队列（保留当前播放）
void Buzzer_ClearQueue(void)
{
    queue_head = 0;
    queue_tail = 0;
    queue_count = 0;
    memset(melody_queue, 0, sizeof(melody_queue));
}

// 计算PWM参数
static void Buzzer_CalculatePWM(uint32_t frequency)
{
    if (frequency == 0)
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
        return;
    }
    
    // STM32H7 TIM3时钟频率计算
    // 系统时钟280MHz，APB1预分频为2，则APB1时钟为140MHz
    // TIM3时钟 = APB1时钟 × 2 = 280MHz
//    uint32_t tim_clock = SystemCoreClock;  // 280MHz
    uint32_t prescaler = htim3.Instance->PSC;
    uint32_t counter_clock = SystemCoreClock / (prescaler + 1);
    
    // 计算ARR值（PWM频率）
    uint32_t arr_value = (counter_clock / frequency) - 1;
    
    // 限制ARR值范围
    if (arr_value > 65535)
    {
        arr_value = 65535;
    }
    if (arr_value < 2)
    {
        arr_value = 2;
    }
    
    // 设置ARR
    htim3.Instance->ARR = arr_value;
    
    // 计算占空比（音量控制）
    uint32_t pulse = (arr_value * buzzer.volume) / 200;
    
    // 设置比较值
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pulse);
    
    // 生成更新事件
    htim3.Instance->EGR = TIM_EGR_UG;
}

// 设置音量
void Buzzer_SetVolume(uint8_t volume_percent)
{
    if (volume_percent > 100)
    {
        volume_percent = 100;
    }
    buzzer.volume = volume_percent;
    
    // 更新当前音调的占空比
    if (buzzer.current_note != BUZZER_SILENCE)
    {
        Buzzer_CalculatePWM(buzzer.current_note);
    }
}

// 设置音调
void Buzzer_SetNote(BuzzerNote_t note)
{
    buzzer.current_note = note;
    
    if (note == BUZZER_SILENCE)
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
    }
    else
    {
        Buzzer_CalculatePWM(note);
    }
}

// 1. 启动旋律示例
static const NoteDuration_t startup_melody_notes[] = 
{
    {BUZZER_NOTE_C5, 100},
    {BUZZER_NOTE_E5, 100},
    {BUZZER_NOTE_G5, 100},
    {BUZZER_SILENCE, 50},
    {BUZZER_NOTE_C6, 200}
};

const Melody_t melody_startup = 
{
    .notes = startup_melody_notes,
    .note_count = sizeof(startup_melody_notes) / sizeof(NoteDuration_t)
};

// 2. 警报旋律示例
static const NoteDuration_t alarm_melody_notes[] = 
{
    {BUZZER_NOTE_A5, 150},
    {BUZZER_SILENCE, 50},
    {BUZZER_NOTE_A4, 150},
    {BUZZER_SILENCE, 50},
    {BUZZER_NOTE_A5, 150},
    {BUZZER_SILENCE, 50},
    {BUZZER_NOTE_A4, 150},
    {BUZZER_SILENCE, 200},
};

const Melody_t melody_alarm = 
{
    .notes = alarm_melody_notes,
    .note_count = sizeof(alarm_melody_notes) / sizeof(NoteDuration_t)
};

// 3. 成功提示旋律示例
static const NoteDuration_t success_melody_notes[] = 
{
    {BUZZER_NOTE_C5, 150},
    {BUZZER_NOTE_C6, 200}
};

const Melody_t melody_success = 
{
    .notes = success_melody_notes,
    .note_count = sizeof(success_melody_notes) / sizeof(NoteDuration_t)
};

// 4. 错误提示旋律示例
static const NoteDuration_t error_melody_notes[] = 
{
    {BUZZER_NOTE_A3, 500},
    {BUZZER_SILENCE, 300},
};

const Melody_t melody_error = 
{
    .notes = error_melody_notes,
    .note_count = sizeof(error_melody_notes) / sizeof(NoteDuration_t)
};

// 5. 按键提示音示例
static const NoteDuration_t button_melody_notes[] = 
{
    {BUZZER_NOTE_C6, 50},
    {BUZZER_SILENCE, 10},
};

const Melody_t melody_button = 
{
    .notes = button_melody_notes,
    .note_count = sizeof(button_melody_notes) / sizeof(NoteDuration_t)
};




