#include "lcd_base.h"
#include "bsp_ltdc.h"
#include "bsp_dma2d.h"

uint16_t LCD_FRAME_BUFFER[LCD_WIDTH * LCD_HEIGHT] __attribute__((used,section(".LCD_FRAME_BUFFER0"), aligned(4)));

void lcd_init(void)
{
    MX_LTDC_Init();
	
#if USE_DMA2D_EN
    MX_DMA2D_Init();
#endif
    
	//初始化完成，打开背光和和显示
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
    
	lcd_clear(COLOR_WHITE);
}

void lcd_clear(uint16_t color)
{
	lcd_rect_fill(0,0,LCD_WIDTH,LCD_HEIGHT,color);
}

/**
 * @brief 使用 DMA2D 执行寄存器到内存（R2M）传输，以指定颜色填充矩形区域。
 * 
 * 该函数配置 DMA2D 为寄存器到内存模式，将固定颜色填充到目标内存的矩形区域中。
 * 适用于快速清屏、绘制单色矩形等场景。
 *
 * @param dst       目标内存的基地址（字节地址），通常指向帧缓冲区。
 * @param width     矩形区域的宽度（像素数）。
 * @param height    矩形区域的高度（像素数）。
 * @param offset    目标内存的行偏移（像素数），用于跳过每行末尾的未使用区域。
 * @param color     填充颜色，格式为 RGB565（16 位）。
 * 
 * @note 调用前需确保 DMA2D 处于空闲状态（该函数内部会等待上一次传输完成）。
 * @note 目标内存地址和偏移量需满足 DMA2D 对齐要求（通常 4 字节对齐）。
 * @note 该函数为阻塞式，会在传输完成前一直等待。
 */
void dma2d_r2m_transfer(uint32_t dst, uint32_t width, uint32_t height, uint32_t offset, uint16_t color)
{
    DMA2D->CR      = DMA2D_R2M;

    DMA2D->OPFCCR  = DMA2D_OUTPUT_RGB565;

    DMA2D->OCOLR   = color;
    DMA2D->OMAR    = dst;
    DMA2D->OOR     = offset;
    DMA2D->NLR     = (width << 16) | (height & 0xFFFF);
    DMA2D->CR     |= DMA2D_CR_START;

    while (DMA2D->CR & DMA2D_CR_START);
}

/**
 * @brief 使用 DMA2D 执行内存到内存（M2M）传输，将源矩形区域复制到目标矩形区域。
 * 
 * 该函数配置 DMA2D 为内存到内存模式，支持源和目标行偏移，可实现任意位置的图像拷贝。
 * 适用于图形渲染、图层合成等场景。
 *
 * @param src    源内存的基地址（字节地址），通常指向包含图像数据的缓冲区。
 * @param dst    目标内存的基地址（字节地址），通常指向帧缓冲区。
 * @param width  矩形区域的宽度（像素数）。
 * @param height 矩形区域的高度（像素数）。
 * 
 * @note 调用前需确保 DMA2D 处于空闲状态（该函数内部会等待上一次传输完成）。
 * @note 源和目标地址及偏移量需满足 DMA2D 对齐要求（通常 4 字节对齐）。
 * @note 源缓冲区需连续存放，行偏移（FGOR）固定为 0；目标行偏移由 LCD_WIDTH - width 自动计算。
 * @note 该函数为阻塞式，会在传输完成前一直等待。
 */
void dma2d_m2m_transfer(uint32_t src, uint32_t dst, uint16_t width, uint16_t height)
{
    DMA2D->CR      = DMA2D_M2M;
	DMA2D->OPFCCR  = DMA2D_OUTPUT_RGB565;
    DMA2D->FGPFCCR = DMA2D_INPUT_RGB565;
    DMA2D->FGOR    = 0;
    DMA2D->OOR     = LCD_WIDTH - width;
    DMA2D->FGMAR   = src;
    DMA2D->OMAR    = dst;
    DMA2D->NLR     = (width << 16) | height;
    DMA2D->CR     |= DMA2D_CR_START;
    while (DMA2D->CR & DMA2D_CR_START);
}

void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    if( x >= LCD_WIDTH || y >= LCD_HEIGHT) return;

    volatile uint16_t *ptr = (volatile uint16_t*)&LCD_FRAME_BUFFER;

    ptr[x + y * LCD_WIDTH] = color;
}

uint16_t lcd_read_point(uint16_t x, uint16_t y)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return 0;
    
    volatile uint16_t *ptr = (volatile uint16_t*)LCD_FRAME_BUFFER;

    return ptr[x + y * LCD_WIDTH];
}

void lcd_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (w == 0 || h == 0) return;
    uint32_t dst = (uint32_t)LCD_FRAME_BUFFER + (y * LCD_WIDTH + x) * LCD_PIXEL_SIZE;
#if USE_DMA2D_EN
    dma2d_r2m_transfer(dst, w, h, LCD_WIDTH - w, color);
#else
    volatile uint16_t *ptr = (volatile uint16_t*)dst;
    for (uint16_t row = 0; row < h; ++row) 
	{
        for (uint16_t col = 0; col < w; ++col) 
		{
            ptr[col] = color;
        }
        ptr += LCD_WIDTH;
    }
#endif
}

