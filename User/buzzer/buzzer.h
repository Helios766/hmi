#ifndef _BUZZER_H_
#define _BUZZER_H_

#include "main.h"

#include "bsp_timer.h"

// 蜂鸣器音调定义（单位：Hz）
typedef enum
{
 // 第3八度 (Octave 3) - 白键
    BUZZER_NOTE_C3 = 338,
    BUZZER_NOTE_D3 = 378,
    BUZZER_NOTE_E3 = 424,
    BUZZER_NOTE_F3 = 450,
    BUZZER_NOTE_G3 = 505,
    BUZZER_NOTE_A3 = 567,
    BUZZER_NOTE_B3 = 637,
    
    // 第4八度 (Octave 4) - 白键
    BUZZER_NOTE_C4 = 675,
    BUZZER_NOTE_D4 = 755,
    BUZZER_NOTE_E4 = 848,
    BUZZER_NOTE_F4 = 899,
    BUZZER_NOTE_G4 = 1010,
    BUZZER_NOTE_A4 = 1134,
    BUZZER_NOTE_B4 = 1274,
    
    // 第5八度 (Octave 5) - 白键
    BUZZER_NOTE_C5 = 1350,
    BUZZER_NOTE_D5 = 1510,
    BUZZER_NOTE_E5 = 1696,
    BUZZER_NOTE_F5 = 1798,
    BUZZER_NOTE_G5 = 2020,
    BUZZER_NOTE_A5 = 2268,
    BUZZER_NOTE_B5 = 2547,
    
    // 第6八度 (Octave 6) - 基准音（最高音）
    BUZZER_NOTE_C6 = 2700,
	
    BUZZER_SILENCE = 0
} BuzzerNote_t;

// 音调持续时间结构体
typedef struct
{
    BuzzerNote_t note;
    uint32_t duration;  // 毫秒
} NoteDuration_t;

// 旋律定义
typedef struct
{
    const NoteDuration_t *notes;  // 音符数组
    uint16_t note_count;          // 音符数量
} Melody_t;


// 优先级枚举
typedef enum
{
    BUZZER_PRIORITY_LOW = 0,      // 低优先级（如按键提示）
    BUZZER_PRIORITY_NORMAL = 1,   // 普通优先级（如成功提示）
    BUZZER_PRIORITY_HIGH = 2,     // 高优先级（如警报）
    BUZZER_PRIORITY_EMERGENCY = 3 // 紧急优先级（打断其他所有声音）
} BuzzerPriority_t;


typedef enum{
    BEEP_Play_SILENCE = 0,	    //静音
    BEEP_Play_Startup,          //启动旋律
    BEEP_Play_Alarm,            //警报旋律
    BEEP_Play_Success,          //成功提示旋律
    BEEP_Play_Error,            //错误提示旋律
    BEEP_Play_Button            //按键提示旋律
}BuzzerType_t;

extern BuzzerType_t Beep_msg;

// 外部声明预定义的旋律
extern const Melody_t melody_startup;
extern const Melody_t melody_alarm;
extern const Melody_t melody_success;
extern const Melody_t melody_error;
extern const Melody_t melody_button;

/*
// 自定义旋律创建宏
#define CREATE_MELODY(name, notes_array) \
const Melody_t name = { \
	.notes = notes_array, \
	.note_count = sizeof(notes_array) / sizeof(NoteDuration_t), \
	.tempo = 120 \
}
*/

// API函数
void Buzzer_Init(void);
void Buzzer_SetVolume(uint8_t volume_percent);
void Buzzer_SetNote(BuzzerNote_t note);
void Buzzer_PlayMelody(const Melody_t *melody, BuzzerPriority_t priority);
void Buzzer_Update(void);
void Buzzer_ClearQueue(void); // 清空队列（保留当前播放）

#endif