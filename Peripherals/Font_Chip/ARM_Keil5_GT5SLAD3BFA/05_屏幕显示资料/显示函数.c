/*横置横排打点函数 -----------------------------------------------------------------*/
/*
	Xpos,Ypos：显示的起点坐标
	data：显示的数据
	charColor：字体颜色
	bkColor：背景颜色
	sizeType：放大倍数，默认为1
*/
void WriteData( uint16_t Xpos, uint16_t Ypos, uint8_t data, uint16_t charColor, uint16_t bkColor,u8 sizeType)
{
		uint16_t j,i;
		unsigned char count=0;
		for( j=0; j<8; j++ )
		{
				if( ((data >> (7-j))&0x01)== 0x01 ){
						for(count=0;count<sizeType;count++){
							for(i=0;i<sizeType;i++){
								LCD_SetPoint( Xpos + sizeType*j+count, Ypos+i, charColor );	//修改此函数
							}
						}
				}
				else{
						for(count=0;count<sizeType;count++){
							for(i=0;i<sizeType;i++){
								LCD_SetPoint( Xpos + sizeType*j+count, Ypos+i, bkColor );	//修改此函数
							}
						}
				}
		}
}

/*竖置横排打点函数 -----------------------------------------------------------------*/
/*
	Xpos,Ypos：显示的起点坐标
	data：显示的数据
	charColor：字体颜色
	bkColor：背景颜色
	sizeType：放大倍数，默认为1
*/
void WriteDataY( uint16_t Xpos, uint16_t Ypos, uint8_t data, uint16_t charColor, uint16_t bkColor,u8 sizeType)
{
		uint16_t i,j;
		unsigned char count = 0;
		for( j = 0; j < 8; j++ )
		{
				if( ((data >> (7-j))&0x01)== 0x01 ){
					for(count = 0;count<sizeType;count++){
						for(i = 0;i < sizeType;i++)
						{
								LCD_SetPoint( Xpos+i, Ypos- sizeType*j+count, charColor );	//修改此函数
						}
					}
				}
				else{
					for(count=0;count<sizeType;count++){
						for(i=0;i<sizeType;i++)
						{
							LCD_SetPoint( Xpos+i, Ypos- sizeType*j+count, bkColor );	//修改此函数
						}
					}
				}
		}
}




/******* 竖置横排显示函数 ***********/
/*
	Xpos,Ypos：显示的起点坐标
	W：字体显示的宽
	H：字体显示的高
	charColor：字体颜色
	bkColor：背景颜色
	DZ_Data：显示的字体数据
	sizeType：放大倍数，默认为1
*/
void DisZK_DZ_Y(uint16_t Xpos, uint16_t Ypos, uint16_t W,uint16_t H, uint16_t charColor, uint16_t bkColor,uint8_t*DZ_Data,u8 sizeType)
{
	static u16 Vertical,Horizontal;
	u32 bit = 0;
	u8 temp = 0;
	Vertical = Ypos + 7*sizeType;
	Horizontal = Xpos;
	for(bit = 0;bit < ((H+7)/8*W);bit++) //data sizeof (byte)
	{
				if((bit%W==0)&&(bit>0))//W/8 sizeof
				{
						temp += 8 * sizeType;
						Horizontal = Xpos;
						Vertical = Ypos + 7*sizeType + temp;
						//Vertical = Ypos + 7*sizeType + temp;
				}
				else if(bit > 0)
				{
						Horizontal += sizeType;
						Vertical = Ypos+7*sizeType + temp;
				}
				WriteDataY(Horizontal,Vertical,DZ_Data[bit],charColor,bkColor,sizeType);
	}
}


/********º横置横排显示函数************/
/*
	Xpos,Ypos：显示的起点坐标
	W：字体显示的宽
	H：字体显示的高
	charColor：字体颜色
	bkColor：背景颜色
	DZ_Data：显示的字体数据
	sizeType：放大倍数，默认为1
*/
void DisZK_DZ_W(uint16_t Xpos, uint16_t Ypos, uint16_t W,uint16_t H, uint16_t charColor, uint16_t bkColor,uint8_t*DZ_Data,u8 sizeType)
{
	static u16 Vertical,Horizontal;
	u32 bit=0;
	Vertical=Ypos;
  Horizontal=Xpos;
	for(bit=0;bit<((W+7)/8*H);bit++) //data sizeof (byte)
	{
				if((bit%((W+7)/8)==0)&&(bit>0))//W/8 sizeof
				{
						Vertical+=sizeType;
						Horizontal=Xpos;
				}
				else if(bit>0)
					Horizontal+=sizeType*8;

				WriteData(Horizontal,Vertical,DZ_Data[bit],charColor,bkColor,sizeType);
	}
}


#if 0
void main() {
	unsigned char zk_buff[256]; //自定义点阵数据空间大小
	gt_16_GetData(0xb0, 0xa1, zk_buff);	//获取点阵数据
	DisZK_DZ_W(0, 0, 16, 16, BLACK, WHITE, zk_buff, 1);	//显示函数
}

///////////////////////////////////////////////////////////////////////////////////

//拉丁文灰度 RGB565
/*
	x,y:显示的起始坐标
	width；宽
	height：高
	DZ_Data:字形数据地址
*/
void DisZK_565_W(u16 x, u16 y, u16 width, u16 height, u8* DZ_Data) {
	u16 tmp_x = x, tmp_y = y;
	u32 Data_Size = width*height, i=0, color = 0;
	while(Data_Size) {
		color = (DZ_Data[i]<<8) + DZ_Data[i+1];
		LCD_Fast_DrawPoint(tmp_x, tmp_y, color);
		i+=2;
		tmp_x++;
		Data_Size--;
		if(tmp_x-x>=width) {
			tmp_x = x;
			tmp_y++;
		}
	}
}

#endif