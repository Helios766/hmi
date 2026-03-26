/**
 @p: 字符点阵数组
 @zfwidth: 	显示字符的宽度
 @zfhigh:	显示字符的高度
 @return:	字符的实际最大宽度
*/
unsigned int get_width(unsigned char *p,unsigned int zfwidth,unsigned int zfhigh )
{
  unsigned char *q;
  unsigned int i,j,tem,tem1,witdh1=0,witdh2=0;
    q=p;
  for (i=0;i<zfwidth/16;i++)
  {
      tem=0;
    tem1=0;
    for (j=0;j<zfhigh;j++)
    {
       tem=(*(q+j*(zfwidth/8)+i*2)|(*(q+1+j*(zfwidth/8)+i*2))<<8);
       tem1=tem1|tem;
    }
    witdh1=0;
    for (j=0;j<16;j++)
    {
      if (((tem1<<j)&0x8000)==0x8000)
      {
        witdh1=j;
      }
    }
    witdh2+=witdh1;
  }
  return witdh2;
}

//
/**
 @p: 字符点阵数组
 @zfwidth: 	显示字符的宽度
 @zfhigh:	显示字符的高度
 @return:	字符的实际最大宽度
*/
unsigned int get_width(unsigned char *p,unsigned int zfwidth,unsigned int zfhigh )
{
	int high_i = 0,width_i = 0;
	unsigned int record = 0;
	unsigned int max_width = 0;

	for(width_i = (zfwidth*2)-1; width_i >= 0; width_i--)
	{
		for(high_i = 0;high_i<=zfhigh;high_i++)
		{

			record = p[(zfwidth*high_i*2)+width_i];

			if(	record )
			{
				max_width = width_i;
				break;
			}

		}
		if(	record )
		{
		//	max_width = width_i;
			break;
		}
	}
	if(width_i>=0)
		return max_width/2+1;
	else
		return zfwidth/3;

}
/**************/


/**
 只适用横置横排
 @p: 字符点阵数组
 @zfwidth: 	显示字符的宽度
 @zfhigh:	显示字符的高度
 @return:	字符的实际最大宽度
*/
unsigned int get_width_1(unsigned char *p,unsigned int zfwidth,unsigned int zfhigh )
{
	unsigned char width_Byte = zfwidth/8;
	unsigned char record = 0;
	unsigned int max_width = 0, cnt=0;
	int i, j, m;

	for(i=width_Byte-1; i>=0; i--)
	{
		record = 0;
		cnt = i;
		for(j=0; j<zfhigh; j++)
		{
			record |= p[cnt];
			cnt += width_Byte;
		}
		if(record != 0)
		{
			break;
		}
	}
	for(m=0; m<8; m++)
	{
		if((record>>m)&0x01)
		{
			max_width = (8-m) + i*8;
			break;
		}
	}
	return max_width;
}
