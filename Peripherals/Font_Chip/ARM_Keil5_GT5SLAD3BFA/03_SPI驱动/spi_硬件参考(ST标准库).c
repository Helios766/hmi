
/******************************************/
//SPI1 pin
#define MCU_SPI1_NSS_PIN		GPIO_Pin_4	//PA4
#define MCU_SPI1_SCK_PIN		GPIO_Pin_5	//PA5
#define MCU_SPI1_MISO_PIN		GPIO_Pin_6	//PA6
#define MCU_SPI1_MOSI_PIN		GPIO_Pin_7	//PA7

#define MCU_SPI1_CS3_PIN		GPIO_Pin_7	//PB7

#define MCU_SPI1_PORT			GPIOA
#define MCU_SPI1_CS3_PORT		GPIOB

#define ZK_SPI1					SPI1
/******************************************/


#define Rom_csH		GPIO_WriteBit(MCU_SPI1_CS3_PORT, MCU_SPI1_CS3_PIN, Bit_SET)
#define Rom_csL		GPIO_WriteBit(MCU_SPI1_CS3_PORT, MCU_SPI1_CS3_PIN, Bit_RESET)
/******************************************/

/**
	SPI1 初始化
*/
void SPI1_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI1_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
												| RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = MCU_SPI1_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//GPIO_Mode_IPD  GPIO_Mode_IN_FLOATING
	GPIO_Init(MCU_SPI1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MCU_SPI1_SCK_PIN|MCU_SPI1_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCU_SPI1_PORT, &GPIO_InitStructure);
		
	#if 0	//硬件NSS, 该部分未测试
	
	GPIO_InitStructure.GPIO_Pin = MCU_SPI1_NSS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(MCU_SPI1_PORT, &GPIO_InitStructure);
	
	#else	//软件模拟CS, 该部分可以正常调用
	
	GPIO_InitStructure.GPIO_Pin = MCU_SPI1_CS3_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MCU_SPI1_CS3_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MCU_SPI1_NSS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(MCU_SPI1_PORT, &GPIO_InitStructure);
	#endif
	
	Rom_csH;	//CS拉高
	
	//SPI模式设置	
	SPI1_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;  //全双工模式			
	SPI1_InitStructure.SPI_Mode=SPI_Mode_Master;										//主设备模式															
	SPI1_InitStructure.SPI_DataSize=SPI_DataSize_8b;									//8字节数据通信												
	SPI1_InitStructure.SPI_CPOL = SPI_CPOL_Low;										//时钟空闲时为低电平		SPI_CPOL_Low
	SPI1_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;										//时钟奇数边沿采样															
	SPI1_InitStructure.SPI_NSS = SPI_NSS_Soft; 										//软件控制NSS(CSN)位								
	SPI1_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 			//通信速率设定，波特率预分频值	SPI_BaudRatePrescaler_256
	SPI1_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;								//数据传输从MSB开始	
	SPI1_InitStructure.SPI_CRCPolynomial = 7; 											//CRC计算多项式							
	SPI_Init(ZK_SPI1, &SPI1_InitStructure);		//初始化
	SPI_Cmd(ZK_SPI1, ENABLE);					//使能.
	
	//Rom_csL;
	//SPI1_ReadWriteByte(0xFF);  
	//Rom_csH;
}
/******************************************/

unsigned char SPI1_ReadWriteByte(unsigned char TxData)
{
	unsigned char RxData;
	uint32_t SPITimeout = 0x1000;
	
	while (SPI_I2S_GetFlagStatus(ZK_SPI1 , SPI_I2S_FLAG_TXE) == RESET)			//检测发送缓存区是否为空
	{
		if((SPITimeout--) == 0) 
			return 0;
	}
	
	#if 01	//直接访问寄存器
	ZK_SPI1->DR = TxData;
	#else
	SPI_I2S_SendData(ZK_SPI1,TxData);												//发送一个字节
	#endif
	
	while (SPI_I2S_GetFlagStatus(ZK_SPI1 , SPI_I2S_FLAG_RXNE) == RESET)			//检测接收缓存区是否为空
 	{
 		if((SPITimeout--) == 0) return 0;
	}
	
	#if 01	//直接访问寄存器
	RxData = ZK_SPI1->DR;
	#else
	RxData = SPI_I2S_ReceiveData(ZK_SPI1);											//存储一个字节
	#endif
	
 	return RxData; 
}


/*******************************************************************************/
//                    Send address sub-pro (STM8,STM32，51)                      
/*******************************************************************************/
static void SPI_Address(unsigned char AddH,unsigned char AddM,unsigned char AddL) 
{
	SPI1_ReadWriteByte(AddH);
	SPI1_ReadWriteByte(AddM);
	SPI1_ReadWriteByte(AddL);
}
/*******************************************************************************/
//                    Get N bytes sub-pro  (STM8,STM32，51)                                    //
/*******************************************************************************/
//客户自己实现，从address地址读取len个字节的数据并存入到DZ_Data数组当中
unsigned long r_dat_bat(unsigned long address,unsigned long DataLen,unsigned char *pBuff)
{
	unsigned long i;
	

	unsigned char addrHigh;
	unsigned char addrMid;
	unsigned char addrLow;
	
	addrHigh=address>>16;
	addrMid=address>>8;
	addrLow=(unsigned char)address;

	Rom_csL;          //片选选中字库芯片

	SPI1_ReadWriteByte(0x03);	//普通读取首先送0X03,然后发送地址高八位addrHigh，中八位addrMid，低八位addrLow。
	SPI_Address(addrHigh,addrMid,addrLow);	

	for(i=0;i<DataLen;i++)
	*(pBuff+i)=SPI1_ReadWriteByte(0x00);

	Rom_csH;
		
	return i;
}
/*******************************/

unsigned char CheckID(unsigned char CMD, unsigned long address,
	unsigned long byte_long,unsigned char *p_arr)
{
	unsigned long j=0;
	
	Rom_csL;
	SPI1_ReadWriteByte(CMD);
	SPI1_ReadWriteByte((unsigned char)((address)>>16));
	SPI1_ReadWriteByte((unsigned char)((address)>>8));
	SPI1_ReadWriteByte((unsigned char)address);
	for(j=0;j<byte_long;j++)
	{
			p_arr[j]=SPI1_ReadWriteByte(0x00);
	}
	Rom_csH;
	
	return 1;
}
/*******************************/

//客户自己实现，从address地址读取一个字节的数据并返回该数据
/**
 * @brief 从address地址读取一个字节的数据
 *
 * @param address 数据所在地址
 * @return unsigned char 该地址的具体数据
 */
unsigned char r_dat(unsigned long address)
{
    unsigned char buff;
    unsigned char addrHigh;
    unsigned char addrMid;
    unsigned char addrLow;

    addrHigh = address >> 16;
    addrMid  = address >> 8;
    addrLow  = (unsigned char)address;

    Rom_csL;          //片选选中字库芯片
    SPI1_ReadWriteByte(0x03);	//普通读取首先送0X03,然后发送地址高八位addrHigh，中八位addrMid，低八位addrLow。
    SPI_Address(addrHigh, addrMid, addrLow);	
    buff = SPI1_ReadWriteByte(0x00);
    Rom_csH;

    return  buff;
}


//注：如何验证r_dat_bat()函数？
//要验证r_dat_bat()函数，可以根据不同的芯片型号读取0xc0/0xb0/0xa0/0x2c0地址的16位数据，并参考以下提供的数据进行对比。如果没有需要的数据，
//请向FAE索要对应型号字库芯片该地址的数据，然后对比读出来的数据是否正确。以下是各个字库型号的0xc0/0xb0/0xa0/0x2c0地址16字节数据以及对应
//型号的地址和地址数据的参考：：
GT5DL14P1Y：c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT5DL14S2Y：c0h: FC 02 02 FC 00 00 01 02 02 01 00 00 00 04 FE 00
GT5DL16M2Y：c0h: 21 72 21 73 21 74 21 75 21 76 21 77 21 78 21 79
GT5DL16S1W：c0h: 00 00 00 00 00 00 0c 60 0c 60 00 00 00 00 00 00
GT5DL24A1Y80：c0h: 00 00 E0 30 18 18 FE 18 18 10 E0 00 00 00 00 00
GT5DL24A2W：c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT5DL28K2W：b0h: CE 00 7B 00 00 00 00 03 00 00 C0 00 C0 00 C0 00
GT5DL32A3W: c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT5SDL24A40: c0h: 00 05 00 00 06 00 11 05 00 00 06 00 22 05 00 00
GT5HL24A2W: 2c0h: A8 AB 00 00 A8 AF 00 00 A8 B3 00 00 A8 B5 00 00
GT5HL32S3W:  2c0h: A8 AB 00 00 A8 AF 00 00 A8 B3 00 00 A8 B5 00 00
GT5HL20K2W: c0h: A8 A4 A8 A2 00 00 00 00 00 00 00 00 00 00 00 00
GT5HL16S2W: 2c0h:A8 B6 00 00 A8 B7 00 00 A8 B8 00 00 00 00 00 00
GT5SL24K3W40: c0h: 68 B0 01 02 78 B0 01 02 88 B0 01 03 A0 B0 01 02
GT5SL24K4W: c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT5SLAD2E1A: c0h: 63 15 00 00 06 00 81 15 00 00 06 00 9F 15 00 00
GT5SLAD3BFA: c0h: 63 15 00 00 06 00 81 15 00 00 06 00 9E 15 00 00
GT5SLCD2E1A: c0h: F7 04 00 00 06 00 08 05 00 00 06 00 19 05 00 00
GT5SLCD2S2A: c0h: 63 15 00 00 06 00 81 15 00 00 06 00 9F 15 00 00
GT5SLCD2S4A: c0h: 68 B0 01 02 78 B0 01 02 88 B0 01 03 A0 B0 01 02
GT5SUAD2E: c0h: 7C 49 01 02 8C 49 01 02 9C 49 01 03 B4 49 01 02
GT5SUAD3BFA: c0h: 63 15 00 00 06 00 81 15 00 00 06 00 9F 15 00 00
GT5SUCD2E1A: c0h: E7 04 00 00 06 00 08 05 00 00 06 00 19 05 00 00
XT5YL14U1Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT5YL24A1Y520: c0h: 00 00 E0 30 18 18 FE 18 18 10 E0 00 00 00 00 00
GT24L24A2Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT20L16J1Y: b0h: 00 00 00 00 00 00 00 03 03 00 00 00 00 00 00 00
GT20L16P1Y: c0h: 00 00 00 00 00 00 00 00 00 00 16 0E 00 00 00 00
GT20L16S1Y: c0h: 00 00 00 00 18 18 00 00 00 18 18 00 00 00 00 00
GT20L24F6Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT21L16S2W: c0h: 00 00 00 00 00 00 0C 60 0C 60 00 00 00 00 00 00
GT21L16S2Y: c0h: 00 00 00 00 18 18 00 00 00 18 18 00 00 00 00 00
GT21L16T1W: c0h: 00 00 00 00 00 00 0C 60 0C 60 00 00 00 00 00 00
GT21L24S1W: c0h: 10 40 00 10 40 00 10 40 00 08 80 00 07 00 00 00
GT22L16A1Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT22L16A2Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT22L16K1Y40: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT22L16M1Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT22L16U1Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT22L16V2Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT22L24S3W: c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT22U16A2Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT23L16U2Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT24L16A2Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT24L16K1Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT24L16M1Y20: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT24L24A2W: c0h: 00 00 00 E0 10 08 08 08 10 E0 00 00 00 00 00 00
GT24L24A2Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT24L32M4W80: c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT24U24A2Y: c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT30L16M2Y: c0h: 00 40 30 1C 17 12 90 70 38 10 00 00 00 00 00 01
GT30L16U2W: c0h: 18 00 04 00 0A 00 31 80 C0 60 00 00 7E C0 01 80
GT30L24A2W: c0h: 0C IC 10 10 10 10 10 1F OF 00 00 00 00 0A 80 C0
GT30L24A3W: c0h: 18 C0 00 18 C0 00 18 C0 00 0F 80 00 07 00 00 00
GT30L24M1W: c0h: 10 40 00 10 40 00 10 40 00 08 80 00 07 00 00 00
GT30L24M1Z: a0h: 40 10 00 40 10 00 40 10 00 80 08 00 00 07 00 00
GT30L24T3Y：c0h: 00 40 30 1C 17 12 90 70 38 10 00 00 00 00 00 01
GT30L32A1W80：d0h: 0c 00 00 00 03 00 00 00 01 CO 00 00 00 E0 00 00
GT30L32M4W80：c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT30L32S4W：c0h: 08 00 0C 00 18 00 10 80 3F CO 21 80 41 00 02 00
GT30L32S4Y：c0h: 00 40 30 1C 17 12 90 70 38 10 00 00 00 00 00 01
GT31L16M1Y80：c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT31L16S2W80：c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT31L24M1W16：c0h: 10 40 00 10 40 00 10 40 00 08 80 00 07 00 00 00
GT31L24M3W40：c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT32L24A180：c0h: 10 40 00 10 40 00 10 40 00 08 80 00 07 00 00 00
GT32L24F0210：c0h: 36 49 49 49 36 00 00 00 06 49 49 29 1E 00 00 00
GT32L24M1Y80：c0h: 00 00 07 0F 18 30 30 30 18 0F 07 00 00 00 00 00
GT32L24M0140：c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT32L32M4W40：c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT32L32M0180：c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT32L32S0140：c0h: 70 88 88 70 88 88 70 00 70 88 88 78 08 10 60 00
GT60L16M2K4：c0h: 00 00 00 00 00 00 00 00 00 18 18 18 30 00 00 00
GT60M2：c0h: 68 B0 01 02 78 B0 01 02 88 B0 01 03 A0 B0 01 02
GT61L24M3K4：c0h: 00 00 00 00 00 00 00 00 00 18 18 18 30 00 00 00
XT21L12S1Y12：c0h: c0 02 C0 01 00 00 00 00 00 00 20 00 20 00 20 00
XT21L12S1Y40：c0h: c0 02 C0 01 00 00 00 00 00 00 20 00 20 00 20 00
XT21L20S2W60：2c0h: 00 00 00 00 00 00 00 00 00 00 00 30 30 10 20 00
XT30L24M1W：c0h: 10 40 00 10 40 00 10 40 00 08 80 00 07 00 00 00



