#define  HZ_MODE00 0//竖置横排(Y)
#define  HZ_MODE01 1//竖置竖排(Z)
#define  HZ_MODE10 2//横置横排(W)
#define  HZ_MODE11 3//横置竖排(X)

#define  HZ_MODE4  4//Y-->W 
//条件putdata的数据写入X，Y的LCD显示缓冲RAM中
/*
*getdate: 转换后的字形数据
*putdate: 转换签订字形数据
*width:	字宽
*high:	字高
*style: 转换功能的选择， HZ_MODE00 0//竖置横排(Y)
						HZ_MODE01 1//竖置竖排(Z)
						HZ_MODE10 2//横置横排(W)
						HZ_MODE11 3//横置竖排(X)
						HZ_MODE4  4//Y-->W 

*使用示例：lcdram_map(getdate,putdate,32,32,HZ_MODE4);//将32x32的字形数据由Y排置（竖置横排）转换成W排置（横置横排）保存在getdate数组中
*/
void lcdram_map( u8 *getdate,u8 *putdata, u8 width, u8 high, u8 style )
{
  u16 i,j,hbyte,wbyte;
  unsigned char i_8,j_8;
  wbyte = (width+7)/8;
  hbyte = (high+7)/8;
   //--------------------------------------------------------------------------
   // Y--> W; Y-->X; Y-->Z;
   //--------------------------------------------------------------------------
  if( style == HZ_MODE4 )	//竖置横排 转 横置横排 ( Y-->W )
  {
	for( i = 0; i < high; i++ )
		for( j = 0; j < width; j++ )
		{
			i_8 = i/8;
			if((*(putdata+i_8*width+j)&(0x01<<(i%8))) > 0)
				getdate[wbyte*i+j/8] |= (0x80>>(j%8));
			else
				getdate[wbyte*i+j/8] &= (~(0x80>>(j%8)));
		}
  }	
   //--------------------------------------------------------------------------
   // W--> Y;  W-->Z;W-->X;
   //--------------------------------------------------------------------------
  
  if( style == HZ_MODE00 )	//竖置横排 (W--> Y)
  {
	  for( i = 0; i < high; i++ )
		  for( j = 0; j < width; j++ )
		  {
			i_8 = i/8;
			if((*(putdata+wbyte*i+j/8)&(0x80>>(j%8))) > 0)
				getdate[i_8*width+j] |= (0x01<<(i%8));
			else
				getdate[i_8*width+j] &= (~(0x01<<(i%8)));
		  }
  }
  else if(style == HZ_MODE01)	//竖置竖排 (W-->Z)
  {
    for( i = 0; i < high; i++ )
      for( j = 0; j < width; j++ )
      {
        if((*(putdata+wbyte*i+j/8)&(0x80>>(j%8))) > 0)
			getdate[j*hbyte+i/8] |= (0x01<<(i%8));
        else
			getdate[j*hbyte+i/8]  &= (~(0x01<<(i%8)));
      }
  }
  else if(style == HZ_MODE11)//横置竖排 (W-->X)
  {
    for( i = 0; i < high; i++ )
      for( j = 0; j < width; j++ )
      {
        j_8 = j/8;
        if((*(putdata+wbyte*i+j/8)&(0x80>>(j%8))) > 0)
			getdate[j_8*high+i] |= (0x80>>(j%8));
        else
			getdate[j_8*high+i] &= (~(0x80>>(j%8)));
      }
  }
  else if(style == HZ_MODE10)//横置横排 做镜像(W-->W')
  {
    for( i = 0; i < high; i++ )
      for( j = 0; j < width; j++ )
      {
        if((*(putdata+wbyte*i+j/8)&(0x80>>(j%8)))>0 )
			*(getdate+wbyte*i+(width-j)/8) |=(0x80>>((width-j)%8));
        else
			*(getdate+wbyte*i+(width-j)/8) &=~(0x80>>((width-j)%8));
      }
  }
}

//竖直竖排转横置横排 Z转W
/*
*getdate: 转换后的字形数据
*putdate: 转换签订字形数据
*width:	字宽
*high:	字高
*/
void Z2W(unsigned char* getData, unsigned char* putData, unsigned char width, unsigned char height) {
	
	unsigned char line, lie;
	unsigned int i=0, count=0, mark = 0;	//i控制putData的下标, count控制getData的下标
	
	for(line=0; line<height; line++) {	//控制行
		for(lie=0; lie<width; lie++) {	//控制列
			getData[count] = (unsigned char)(getData[count]<<0x01);	//准备接收下一下
			if(putData[i]&(0x01<<(line%8))) 
				getData[count] += 1;	//先取低位, 存新数组内
			else
				getData[count] += 0;
			i += height>>3;	//下一列
			if(lie%8 == 7) {
				count++;
			}
		}
		if(line%8 == 7) {
			i = ++mark;
		}else
			i = mark;
	}
}

//竖直竖排转竖置横排(Z->Y)
/*
*getdate: 转换后的字形数据
*putdate: 转换签订字形数据
*width:	字宽
*high:	字高
*/
void Z2Y(unsigned char* getData, unsigned char* putData, unsigned char width, unsigned char height) {
	unsigned char line=height>>3;
	unsigned int i=0, count=0, mark_count=0, sum = width*height>>3;

	for(i=0;i<sum;i++) {
		getData[count] = putData[i];
		count += width;
		if( ((i+1)%line) == 0)
			count = ++mark_count;
	}
}

//竖置横排转竖直竖排  Y转Z
/*
*getdate: 转换后的字形数据
*putdate: 转换签订字形数据
*width:	字宽
*high:	字高
*/
void Y2Z(unsigned char* getData, unsigned char* putData, unsigned char width, unsigned char height) {
	unsigned char line=height>>3;
	unsigned int i=0, count=0, mark_count=0, sum=width*height>>3;
	
	for(i=0; i<sum; i++) {
		getData[count] = putData[i];
		count += line;
		if( ((i+1)%width) == 0 ) {
			count = ++mark_count;
		}
	}
}




