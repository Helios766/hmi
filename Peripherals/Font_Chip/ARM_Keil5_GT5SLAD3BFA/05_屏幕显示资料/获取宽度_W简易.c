/**
 * @brief 得到灰度文字的宽度 (W横置横排)
 *
 * @param pdata 文字数据
 * @param dot_w 文字宽度     例如：48x48大小的文字 dot_w = 48
 * @param dot_h 文字高度         例如：48x48大小的文字 dot_h = 48
 * @param gray 取值: 1 / 2 / 4
 * @return int 实际文字宽度
 */
int _get_font_width(uint8_t *pdata, uint16_t dot_w, uint16_t dot_h, uint16_t gray)
{
    uint16_t byte_w, i;
    int j, len;
    uint8_t tmp = 0;

    byte_w = ((dot_w + 7) >> 3) * gray;

    for (j = byte_w - 1; j >= 0; j--)
    {
        tmp = 0;
        for (i = 0; i < dot_h; i++)
        {
            tmp |= pdata[i * byte_w + j];
        }
        if (0 != tmp)
        {
            for (i = 0; i < 8; i++) {
                if ((tmp >> i) & 0x01) {
                    break;
                }
            }

            len = ((j / gray) * 8) + ((j % gray) * 8 + (8 - i)) / gray;
            //
            if (len >= dot_h) {
                continue;
            }

            return len;
        }
    }

    return dot_w;
}

————————————————
版权声明：本文为CSDN博主「@YangC」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/qq_41382608/article/details/130639791