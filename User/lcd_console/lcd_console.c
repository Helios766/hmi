#include "lcd_console.h"
#include "lcd_base.h"

uint8_t LCD_FRAME_BUFFER1[LCD_WIDTH * LCD_HEIGHT * LCD_PIXEL_SIZE] __attribute__((used,section(".LCD_FRAME_BUFFER1"), aligned(4)));