#ifndef __DEBUG_LED_H__
#define __DEBUG_LED_H__

#include "main.h"


void LED_Debug_Update(void);
void debug_led_init(void);
void debug_led_set(uint16_t on_time,uint16_t off_time);

#endif