
/*
	功能：  UTF-8编码转UTF-16编码
	pUTF8： UTF-8编码数据
	pUTF16：UTF-16编码数据
	return：UTF-16字符的字节长度
*/
unsigned char UTF8toUTF16(unsigned char *pUTF8, unsigned short *pUTF16)
{
	unsigned char bytes[3];
	unsigned short unicode16;
	bytes[0]=*pUTF8++;
	if(bytes[0]<0x80)
	{
		*pUTF16=bytes[0];
		return (1);
	}
	bytes[1]=*pUTF8++;
	if(bytes[0]>=0xC0&&bytes[0]<0xE0)
	{
		unicode16=0x1f&bytes[0];
		*pUTF16=(unicode16<<6)+(0x3f&bytes[1]);
		return (2);
	}
	bytes[2]=*pUTF8++;
	if(bytes[0]>=0xE0 &&bytes[0]<0xF0)
	{
		unicode16=0x0f&bytes[0];
		unicode16=(unicode16<<6)+(0x3f&bytes[1]);
		*pUTF16=(unicode16<<6)+(0x3f&bytes[2]);
		return (3);
	}
	else
	{
		return(0);
	}
}

/**
 * utf-8 到 unicode转码
 * @param p_utf8	utf8数组
 * @param res		返回的unicode编码
 * @return			utf8字符的字节长度
 */
uint8_t utf8_to_unicode(uint8_t *p_utf8, uint32_t *res)
{
	if (p_utf8[0] < 0x80) {
		*res = p_utf8[0];
		return 1;
	} else if (p_utf8[0] < 0xE0) {
		*res = ((p_utf8[0] & 0x1F) << 6) | (p_utf8[1] & 0x3F);
		return 2;
	} else if (p_utf8[0] < 0xF0) {
		*res = ((p_utf8[0] & 0x0F) << 12) | ((p_utf8[1] & 0x3F) << 6) | (p_utf8[2] & 0x3F);
		return 3;
	} else if (0xF0 == (p_utf8[0] & 0xF0)) {
		*res = ((p_utf8[0] & 0x07) << 18) | ((p_utf8[1] & 0x3F) << 12) | ((p_utf8[2] & 0x3F) << 6) | (p_utf8[3] & 0x3F);
		return 4;
	}

	return 0;
}







