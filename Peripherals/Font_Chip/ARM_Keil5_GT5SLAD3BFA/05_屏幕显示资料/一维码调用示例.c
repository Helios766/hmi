#include "stdio.h"
#include "string.h"

/**************************************
arr[] : 13个数组
**************************************/
void  BAR_codeEAN13(unsigned int x, unsigned int y, unsigned char arr[])
{
	unsigned char buff_temp[60];
	unsigned long *tmp ;
	unsigned char i = 0;

	tmp = BAR_CODE_EAN13(arr);

	for(i = 0;i < 13;i++)
	{
		memset(buff_temp,0,sizeof(buff_temp));
		r_dat_bat(*tmp, 54, buff_temp);
		DisZK_DZ_W(x,y,16,28,BLACK,WHITE,&buff_temp[2],1);
		x+=buff_temp[1];
		tmp++;
	}
}


/**************************************
ascii_arr[] : 字符数组 ASCII
arr_len:	字符数组的长度
Code 39码只接受如下43个有效输入字符：
1. 26个大写字母(A – Z);
2. 十个数字(0 – 9)；
3. 连接号(-),句号(.),空格,美圆符号($),斜扛(/),加号(+)以及百分号(%)。其余的输入将被忽略。
**************************************/
void BAR_codeEAN39(unsigned int x, unsigned int y, unsigned char ascii_arr[], unsigned int arr_len) {
	unsigned char buff_tmp[50], i;
	
	for(i=0; i<arr_len; i++) {
		BAR_CODE39(ascii_arr[i], buff_tmp);
		DisZK_DZ_W(x,y,16,22,BLACK,WHITE,&buff_tmp[0],1);
		x+=11;
	}
}

/**************************************
arr[]: 条形码编码 整数
arr_len: 数组长度
flag: 起始符有3种模式
	当flag=1时为Code-128-A;
	当flag=2时为Code-128-B;
	当flag=3时为Code-128-C;
Code128编码表参考: https://blog.csdn.net/rodulf/article/details/51276820
**************************************/
void BAR_codeEAN128(unsigned int x, unsigned y, unsigned char arr[], unsigned int arr_len, unsigned char flag) {
	unsigned char buff_tmp[60];
	unsigned long *tmp;
	unsigned char i=0;
	const unsigned char verify = 3;
	
	tmp = BAR_CODE128(arr, arr_len, flag);
	
	for(i=0; i<arr_len+verify; i++) {
		memset(buff_tmp, 0, sizeof(buff_tmp));
		r_dat_bat(*tmp, 40, buff_tmp);
		DisZK_DZ_W(x, y, 16, 20, BLACK, WHITE, &buff_tmp[0], 1);	//显示函数, 根据实际修改, 参考显示函数文件
		x+=11;	//可修改
		tmp++;
	}
}

//
#if 0
int main(void) {
	
	unsigned char code[13]={6,9,0,1,6,8,7,5,8,2,4,4,2};
	unsigned char code1[13]={6,9,3,8,7,0,9,3,0,0,4,4,9};
	unsigned char code2[13]={'A','1','9','3','4','7','$',' ','-','5','8','3','E'};
	unsigned char code3[13] = {16,33,45,31,27,26,40};
	
	#if 01
	BAR_codeEAN13(0, 0, code);
	BAR_codeEAN13(0, 0, code1);
	#endif
	
	#if 01
	BAR_codeEAN39(0, 150, code2, 13);
	#endif
	
	#if 01
	BAR_codeEAN128(0, 200, code3, 7, 1);
	#endif
	
	return 0;
}

#endif





