#define DISMODE 0 // 1  刷RAM显示  0 打点显示
//描点函数
#if DISMODE
void LCD_WriteRAM(unsigned short RGB_Code)
{
	LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}
#else //!DISMODE
static void Gui_DrawPoint(unsigned int x, unsigned int y, unsigned int color)
{
	//显示一个点
	LCD_Fast_DrawPoint(x, y, color);//用户根据自身定义更换
}
#endif //!DISMODE

#define GT_COLOR_GET_R(COLOR)    (((COLOR) >> 11) & 0x1f)
#define GT_COLOR_GET_G(COLOR)    (((COLOR) >> 5) & 0x3f)
#define GT_COLOR_GET_B(COLOR)    (((COLOR)) & 0x1f)
#define GT_UDIV255(x)    (((x) * 0x8081U) >> 0x17)
/**
 * @brief 16位灰度颜色混合
 * @param bg_color 背景色
 * @param ft_color 字体颜色
 * @param gray_scale 灰度值
 * @return unsigned short
 */
static unsigned short gt_color_mix(unsigned short bg_color, unsigned short ft_color, unsigned char gray_scale)
{
    unsigned short mix_color = 0, temp = 0;

    //R
    temp = GT_UDIV255(GT_COLOR_GET_R(ft_color) * gray_scale + GT_COLOR_GET_R(bg_color) * (0xff - gray_scale));
    mix_color = (temp & 0x1f) << 11;

    //G
    temp = GT_UDIV255(GT_COLOR_GET_G(ft_color) * gray_scale + GT_COLOR_GET_G(bg_color) * (0xff - gray_scale));
    mix_color = mix_color | ((temp & 0x3f) << 5);

    //B
    temp = GT_UDIV255(GT_COLOR_GET_B(ft_color) * gray_scale + GT_COLOR_GET_B(bg_color) * (0xff - gray_scale));
    mix_color = mix_color | (temp & 0x1f);

    return mix_color;
}
/**
 * @brief 灰度矢量文字 底色混合 显示函数
 * @param pdata  灰度文字数据
 * @param x_y_w_h  x,y 显示起始坐标; w 宽度, h 高度
 * @param grade  灰度阶级[1阶/2阶/3阶/4阶]
 * @param bg_color  背景颜色
 * @param ft_color  字体颜色
 */
void display_gray_vector_font(unsigned char *pdata, unsigned short x, unsigned short y, unsigned short w, unsigned short h,
        unsigned char grade, unsigned short bg_color, unsigned short ft_color)
{
    unsigned short w_byte = (w + 7) >> 3;
    unsigned short x_s = x, idx = 0, i = 0, j = 0;
    unsigned short mix_color = 0;
    unsigned char gray_data = 0, gray_scale = 0;
    w = w_byte << 3;

#if DISMODE
	LCD_Set_Window(x, y, w, h);	//设置窗口
	LCD_WriteRAM_Prepare();		//开始写入GRAM
#endif //!DISMODE

    if (2 == grade || 4 == grade)
    {
        for (i = 0; i < h * w_byte * grade; i++)
        {
            gray_data = pdata[idx++];//取一个字节灰度数据
            for (j = 0; j < (8 / grade); j++) {
                gray_scale = (gray_data >> (8 - grade)) * (0xFF / ((1 << grade) - 1)); //获取该点灰阶值
                gray_data <<= grade;

                if (gray_scale == 0) { //获取该点颜色值
                    mix_color = bg_color;
                }
                else if (0xff == gray_scale) {
                    mix_color = ft_color;
                } else {
                    mix_color = gt_color_mix(bg_color, ft_color, gray_scale);
                }

            #if DISMODE //显示
                //写RAM的方式16位数据
                LCD_WriteRAM(mix_color);
            #else //!DISMODE
                if (x < x_s + w) {
                    Gui_DrawPoint(x, y, mix_color);    //打点方式
                    x++ ;
                } else {
                    x = x_s;
                    y++;
                    Gui_DrawPoint(x, y, mix_color);    //打点方式
                    x++;
                }
            #endif //!DISMODE
            }
        }
    }
    else if (3 == grade) {
        for (i = 0; i < h * w_byte; i++)
        {
            gray_data = pdata[idx];//取一个字节灰度数据
            for (j = 0; j < 8; j++) {
                if (2 == j) {//计算灰阶值
                    gray_data = (pdata[idx] & 0x03) << 1;
                    idx++;
                    gray_scale = (gray_data | (pdata[idx] >> 7)) * (0xFF / ((1 << 3) - 1)); //(前字节后两位 | 后字节前1位) * 每份比例
                    gray_data = pdata[idx] << 1;
                }
                else if (5 == j) {
                    gray_data = (pdata[idx] & 0x01) << 2;
                    idx++;
                    gray_scale = (gray_data | (pdata[idx] >> 6)) * (0xFF / ((1 << 3) - 1));//(前字节后1位 | 后字节前2位) * 每份比例
                    gray_data = pdata[idx] << 2;
                } else {
                    gray_scale = (gray_data >> 5) * (0xFF / ((1 << 3) - 1));
                    gray_data <<= 3;
                }

                if (gray_scale == 0) { //根据灰阶值获取该点颜色值
                    mix_color = bg_color;
                }
                else if (0xfc == gray_scale) {   //0xff/7*7 = 0xfc
                    mix_color = ft_color;
                } else {
                    mix_color = gt_color_mix(bg_color, ft_color, gray_scale);
                }
            #if DISMODE //显示
                //写RAM的方式16位数据
                LCD_WriteRAM(mix_color);
            #else //!DISMODE
                if (x < x_s + w) {
                    Gui_DrawPoint(x, y, mix_color);    //打点方式
                    x++ ;
                } else {
                    x = x_s;
                    y++;
                    Gui_DrawPoint(x, y, mix_color);    //打点方式
                    x++;
                }
            #endif //!DISMODE
            }
            idx++;
        }
    } else {
        for (i = 0; i < h * w_byte; i++)
        {
            gray_data = pdata[idx++];//取一个字节灰度数据
            for (j = 0; j < 8; j++)
            {
                if (gray_data & 0x80) {
                    mix_color = ft_color;
                } else {
                    mix_color = bg_color;
                }
                gray_data <<= 1;
            #if DISMODE //显示
                //写RAM的方式16位数据
                LCD_WriteRAM(mix_color);
            #else //!DISMODE
                if (x < x_s + w) {
                    Gui_DrawPoint(x, y, mix_color);    //打点方式
                    x++ ;
                } else {
                    x = x_s;
                    y++;
                    Gui_DrawPoint(x, y, mix_color);    //打点方式
                    x++;
                }
            #endif //!DISMODE
            }
        }
    }
}
