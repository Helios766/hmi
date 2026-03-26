#ifndef __LCD_CONSOLE_H
#define __LCD_CONSOLE_H


#include "main.h"
#include "bsp_ltdc.h"

/* LCD 相关定义 */
#define USE_DMA2D_EN        1   /* 启用 DMA2D 加速 */

/* 颜色定义 (RGB565) */
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_YELLOW    0xFFE0
#define COLOR_GRAY      0x8410


/* 底层图形函数（已在其他文件实现） */
void lcd_init(void);
void lcd_clear(uint16_t color);
void dma2d_r2m_transfer(uint32_t dst, uint32_t width, uint32_t height, uint32_t offset, uint16_t color);
void dma2d_m2m_transfer(uint32_t src, uint32_t dst, uint16_t width, uint16_t height);
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);
uint16_t lcd_read_point(uint16_t x, uint16_t y);
void lcd_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#endif /* __LCD_CONSOLE_H */