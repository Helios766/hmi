/*
    p:需要逆置的数据地址
    len:需要逆置的数据长度
*/
void byte_cov(unsigned char *p,unsigned long len)
{
    unsigned long i;
    unsigned char tem;
    for (i=0;i<len;i++)
    {
        tem=p[i];
        p[i]=((tem & 0x01)<<7)|((tem & 0x02)<<5)|((tem & 0x04)<<3)|((tem & 0x08)<<1)  \
            |((tem & 0x10)>>1)|((tem & 0x20)>>3)|((tem & 0x40)>>5)|((tem & 0x80)>>7);
    }
}

/**
 * @brief Y排置字体镜像
 * dz_data：字形数据
 * len：数据长度
 */
void y_mirror(unsigned char *dz_data, unsigned long len)
{
#if 0
    unsigned long i;
    unsigned char tem;

    for (i = 0; i < len; i++)
    {
        tem = dz_data[i];
        dz_data[i] = dz_data[len - i - 1];
        dz_data[len - i - 1] = tem;
    }
#else
    unsigned long i, mid;
    unsigned char tem;

    mid = len >> 1;

    for (i = 0; i < mid; i++)
    {
        tem = dz_data[i];
        dz_data[i] = dz_data[mid - 1 - i];
        dz_data[mid - 1 - i] = tem;
    }

    for (i = mid; i < len; i++)
    {
        tem = dz_data[i];
        dz_data[i] = dz_data[len - 1 - i];
        dz_data[len - 1 - i] = tem;
    }

    for (i = 0; i < mid; i++)
    {
        tem = dz_data[i];
        dz_data[i] = dz_data[mid + i];
        dz_data[mid + i] = tem;
    }
#endif
}


/**
 * @brief Y排置字体镜像
 * dz_data：字形数据
 * len：数据长度
 */
void y_mirror(unsigned char *dz_data, unsigned long len)
{
unsigned long i, mid;
unsigned char tem;

mid = len >> 1;

for (i = 0; i < mid; i++)
{
tem = dz_data[i];
dz_data[i] = dz_data[mid - 1 - i];
dz_data[mid - 1 - i] = tem;
}

for (i = mid; i < len; i++)
{
tem = dz_data[i];
dz_data[i] = dz_data[len - 1 - i];
dz_data[len - 1 - i] = tem;
}

for (i = 0; i < mid; i++)
{
tem = dz_data[i];
dz_data[i] = dz_data[mid + i];
dz_data[mid + i] = tem;
}
}