/**
 * @brief 发送读取函数
 *
 * @param sendbuf 发送数据的buff
 * @param sendlen 发送数据长度
 * @param receivebuf 读取数据的buff
 * @param receivelen 读取数据长度
 */
unsigned char gt_read_data(unsigned char * sendbuf, unsigned char sendlen, unsigned char * receivebuf, unsigned int receivelen)
{
    unsigned int i;
    Rom_csL;    //拉低片选cs，选中flash
    for(i = 0; i < sendlen; i++)
    {
        SPI1_ReadWriteByte(sendbuf[i]); //发送数据
    }
    for(i = 0; i < receivelen; i++)
    {
        receivebuf[i] = SPI1_ReadWriteByte(0x00);   //接收数据，保存到receivebuf[]
    }
    Rom_csH;    //拉高片选cs，空间
    return 1;
}

//注：如何验证gt_read_data()是正常工作的?
//使用如下代码验证，看tmp_buf里面的数据。如果tmp_buf里面的数据不是0或0xff, 而是有规律的一组数字，前三位的最后一位在12-19范围内或者是C9, 
//则是大概率是正确的。数据正确后并实现了r_dat_bat()函数，r_dat_bat()实现方式请参考本文件夹其余3个文档，根据自己的情况选其中一个实现
//即可则可以尝试在SPI初始化后加入GT_Font_Init函数，如果GT_Font_Init返回值大于0则表示字库初始化成功。
  uint8_t tmp_buf[8] = {0};
  tmp_buf[0] = 0x9F;
  gt_read_data(tmp_buf , 1 , tmp_buf , 8);
  for(int i =0;i<8;i++)
  {
    printf("tmp_buf:%x\r\n",tmp_buf[i]);
  }

