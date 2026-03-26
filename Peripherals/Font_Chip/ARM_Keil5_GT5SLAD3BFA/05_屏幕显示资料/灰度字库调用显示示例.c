/**
 * @file 灰度字库调用显示示例.c
 * @author your name (your@email.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-09 17:12:57
 * @copyright Copyright (c) 2014-present, Company Genitop. Co., Ltd.
 */

/* macro ----------------------------------------------------------------*/

#define DISMODE 1 // 1  刷图显示  0 打点显示

/* private define -------------------------------------------------------*/

/*******************************************************************************/
/*   						 lcd的写屏函数 									    */
/*******************************************************************************/
/*
	//开始写GRAM
	void LCD_WriteRAM_Prepare(void)
	{
		LCD->LCD_REG=lcddev.wramcmd;	  
	}	 
	//RGB_Code:颜色值
	void LCD_WriteRAM(u16 RGB_Code)
	{							    
		LCD->LCD_RAM = RGB_Code;//写十六位GRAM
	}
*/
/* private typedef ------------------------------------------------------*/



/* static variables -----------------------------------------------------*/

//描点函数 
#if DISMODE == 0
static void Gui_DrawPoint(unsigned int x, unsigned int y, unsigned int Data)
{
	//显示一个点
	putPixel( x, y, Data);
}
#endif

/*******************************************************************************/
/*   以下函数为灰度矢量字库所有，这里只做示例作用，实际调用需根据所使用的字库来修改   */
/*******************************************************************************/
/*
 *函数名：	get_font()
 *功能：		矢量文字读取函数
 *参数：pBits		数据存储
 *		sty			文字字体选择
 *		fontCode	文字编码
 *		width		文字宽度
 *		height		文字高度
 *		thick		文字粗细
        Grade		字符的灰度阶级[1阶/2阶/3阶/4阶]
*返回值：文字显示宽度
**/
unsigned int get_font(u nsigned char *pBits,unsigned char sty,unsigned long fontCode,unsigned char width,unsigned char height, unsigned char thick);
/********************* 矢量区域结束 *********************/


/*
 *函数名：	get_Font_Gray()
 *功能		灰度矢量文字读取函数
 *参数：pBits		数据存储
 *		sty			文字字体选择  @矢量公用部分
 *		fontCode	字符编码中文:GB18030, ASCII/外文: unicode
 *		fontSize	文字大小
 *		thick		文字粗细
 *返回值：re_buff[0] 字符的显示宽度 , re_buff[1] 字符的灰度阶级[1阶/2阶/3阶/4阶]
**/
unsigned int* get_Font_Gray(unsigned char *pBits,unsigned char sty,unsigned long fontCode,unsigned char fontSize, unsigned char thick);


/*----------------------------------------------------------------------------------------
 * 灰度数据转换函数 2阶灰度/4阶灰度
 * 说明 : 将点阵数据转换为灰度数据 [eg:32点阵数据转2阶灰度数据则转为16点阵灰度数据]
 * 参数 ：
 *   OutPutData灰度数据;	 width 宽度; High 高度;	grade 灰度阶级[1阶/2阶/3阶/4阶]
 *------------------------------------------------------------------------------------------*/
void Gray_Process(unsigned char *OutPutData ,int width,int High,unsigned char Grade);

/* class ----------------------------------------------------------------*/



/* static functions -----------------------------------------------------*/



/* global functions / API interface -------------------------------------*/

/*----------------------------------------------------------------------------------------
 * 灰度数据显示函数 1阶灰度/2阶灰度/4阶灰度 
 * 参数 ：
 * data灰度数据;  x,y=显示起始坐标 ; w 宽度, h 高度,grade 灰度阶级[1阶/2阶/4阶]
 * HB_par	1 白底黑字	0 黑底白字
 *------------------------------------------------------------------------------------------*/
void Gray_Display_hz(unsigned char *data,unsigned short x,unsigned short y, 
	unsigned short w ,unsigned short h,unsigned char grade, unsigned char HB_par)
{       
	unsigned int temp=0,gray,x_temp=x;
	unsigned int i=0,j=0,k=0,t;
	unsigned char c,c2,*p;
	unsigned long color8bit,color4bit,color3bit[8],color2bit,color;

	t=(w+7)/8*grade;//
	p=data;
		t=(w+7)/8*4;//
	#if DISMODE
	LCD_Set_Window(x,y,(((w+7)/8)*8),h);	//设置窗口
	LCD_WriteRAM_Prepare();					//开始写入GRAM
	#endif
	
	if(grade==2)	//2bits 
	{
		for(i=0;i<t*h;i+=2)
		{
			c=*p++;
			for(j=0;j<4;j++)
			{
				color2bit=(c>>6);						//获取像素点的2bit颜色值
				if(HB_par==1)
					color2bit= (3-color2bit)*250/3;		//白底黑字
				else 
					color2bit= color2bit*250/3;			//黑底白字
				gray=color2bit/8;

				color=(0x001f&gray)<<11;				//r-5
				color=color|(((0x003f)&(gray*2))<<5);	//g-6
				color=color|(0x001f&gray);				//b-5
				temp=color;
				temp=temp;
				c<<=2;
				
				#if DISMODE
				//写16位数据
				LCD_WriteRAM(temp);		
				#else
				//打点				
				if(x<(x_temp+w))
				{
					Gui_DrawPoint(x,y,temp);	
				}
				x++;
				if(x>=x_temp+(w+7)/8*8) {x=x_temp; y++;}
				#endif
			}
			
		}
	}
	else if(grade==3)//3bits 
	{
		for(i=0;i<t*h;i+=3)
		{
			c=*p; c2=*(p+1);
			color3bit[0]=(c>>5)&0x07;
			color3bit[1]=(c>>2)&0x07;
			color3bit[2]=((c<<1)|(c2>>7))&0x07;
			p++;
			c=*p; c2=*(p+1);
			color3bit[3]=(c>>4)&0x07;
			color3bit[4]=(c>>1)&0x07;
			color3bit[5]=((c<<2)|(c2>>6))&0x07;
			p++;
			c=*p; 
			color3bit[6]=(c>>3)&0x07;
			color3bit[7]=(c>>0)&0x07;
			p++;
			for(j=0;j<8;j++)
			{
				if(HB_par==1)
				color3bit[j]= (7-color3bit[j])*255/7;	//白底黑字
				else 
				color3bit[j]=color3bit[j]*255/7;		//黑底白字
				gray =color3bit[j]/8;
				color=(0x001f&gray)<<11;				//r-5
				color=color|(((0x003f)&(gray*2))<<5);	//g-6
				color=color|(0x001f&gray);				//b-5
				temp =color;
				
				#if DISMODE
				//写16位数据
				LCD_WriteRAM(temp);	
				#else
				//打点
				if(x<(x_temp+w))
				{
					Gui_DrawPoint(x,y,temp);	
				}
				x++;
				if(x>=x_temp+(w+7)/8*8) {x=x_temp; y++;}
				#endif
			}
		}
	}
	else if(grade==4)	//4bits 
	{
		for(i=0;i<t*h;i++)
		{
			c=*p++;
			for(j=0;j<2;j++)
			{
				color4bit=(c>>4);
				if(HB_par==1)
					color4bit= (15-color4bit)*255/15;	//白底黑字
				else 
					color4bit= color4bit*255/15;		//黑底白字
				gray=color4bit/8;
				color=(0x001f&gray)<<11;				//r-5
				color=color|(((0x003f)&(gray*2))<<5);	//g-6
				color=color|(0x001f&gray);				//b-5
				//printf("color = 0x%X\r\n",color4bit);
				temp=color;
				c<<=4;
				
				#if DISMODE
				//写16位数据
				LCD_WriteRAM(temp);			
				#else
				//打点
				if(x<(x_temp+w)){	
					Gui_DrawPoint(x,y,temp);	
				}
				x++;
				if(x>=x_temp+(w+7)/8*8) {x=x_temp; y++;}
				#endif
			}
			
		}
	}
	
	else if(grade == 5 || grade == 6)
	{
	 for(i=0;i<t*h;i++)
		{
			c=*p++;
			for(j=0;j<2;j++)
			{
				color4bit=(c>>4);
				if(HB_par==1)
					color4bit= (15-color4bit)*255/15;	//白底黑字
				else 
					color4bit= color4bit*255/15;		//黑底白字
				
				if(color4bit > 0x00 && color4bit < 0xFF && grade == 5)
				  color4bit = color4bit - 0x11;
			  else if(color4bit >= 0x11 && color4bit < 0xFF && grade == 6)
					color4bit =    (color4bit == 0x11) ? (color4bit - 0x11) : (color4bit - 0x22);
				
				gray=color4bit/8;
				color=(0x001f&gray)<<11;				//r-5
				color=color|(((0x003f)&(gray*2))<<5);	//g-6
				color=color|(0x001f&gray);				//b-5
				//printf("color = 0x%X\r\n",color4bit);
				temp=color;
				c<<=4;
				
				#if DISMODE
				//写16位数据
				LCD_WriteRAM(temp);			
				#else
				//打点
				if(x<(x_temp+w)){	
					Gui_DrawPoint(x,y,temp);	
				}
				x++;
				if(x>=x_temp+(w+7)/8*8) {x=x_temp; y++;}
				#endif
			}
			
		}
	
	
	}
	else	//1bits
	{
		for(i=0;i<t*h;i++)
		{
			c=*p++;
			for(j=0;j<8;j++)
			{
				if(c&0x80) color=0x0000;
				else color=0xffff;
				c<<=1;
				
				#if DISMODE
				//写16位数据
				LCD_WriteRAM(color);
				#else
				//打点
				if(x<(x_temp+w))
				{
					if(color == 0x0000 && HB_par == 1)
					{				
						Gui_DrawPoint(x,y,color);	//打点
					}
					else if(HB_par == 0 && color == 0x0000)
					{
						Gui_DrawPoint(x,y,~color);	//打点
					}
				}
				x++;
				if(x>=x_temp+(w+7)/8*8) {x=x_temp; y++;}
				#endif
			}
		}
	}
}




int main(void)
{
    unsigned char dataResult2[1024] = {0};
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     //设置系统中断优先级分组2
    delay_init(72);                                     //初始化延时函数
    uart_init(115200);	                                //初始化串口波特率为115200
    LED_Init();				 	                        //初始化LED 
    LED0 = 1;
    LCD_Init();					                        //LCD初始化 
    KEY_Init(); 				                        //按键初始化  
    //	W25QXX_Init();			                        //W25QXX初始化
    //  GPIO_Init_time();
    GT_SPI_init();
    LCD_Display_Dir(1);		                            //默认为横屏显示
    LCD_Clear(BLUE);                                    //清屏为蓝色 	

    Read_ID();		                                    //显示芯片ID

    int back = GT_Font_Init();                          //字库初始化
    printf("back = %d\r\n",back);
    sprintf((char*)dataResult2, "GT_Font_Init = %d = 0x%x", back, back);
    LCD_ShowString(100, 20, 230, 16, 16, dataResult2);  //显示字库初始化返回值和字库芯片的ID

    unsigned char HZ_pBits[5000] = {0};
    unsigned int re_buff[2] = {0};
	unsigned char grade_hz = 2;                         //字符的灰度阶级为2

	//以下是两个灰度矢量字符数据的调用示例
#if 01
	int dot_hz = get_font(HZ_pBits, VEC_BLACK_STY, 0xb2e2, 68*grade_hz, 68*grade_hz, 190);  //获取矢量字符的数据，保存在HZ_pBits数组中
	printf("re_buff = %d\r\n", dot_hz);
	dot_hz = dot_hz/grade_hz;
	Gray_Process(HZ_pBits, dot_hz, 68, grade_hz);       //将矢量字符的数据转换为grade_hz灰度阶级的灰度数据
#else 
	unsigned int * re = NULL;
	re = get_Font_Gray(HZ_pBits, VEC_BLACK_STY, 0xb2e2, 28, 120);		//获取灰度矢量字符数据，保存在HZ_pBits数组中
	printf("re = %d\r\n", re[0]);
#endif

    //显示grade_hz灰度阶级的灰度数据所表示的字符，具体显示函数需根据实际的lcd显示来修改
	Gray_Display_hz(HZ_pBits, 100,  100, dot_hz, 68, grade_hz, 1);  


    return 0;
}


/* end ------------------------------------------------------------------*/



