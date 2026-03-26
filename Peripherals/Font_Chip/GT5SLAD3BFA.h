/**
 * @file GT5SLAD3BFA.h
 * @author D.
 * @brief
 * @version 1.2
 * @date 2023-09-14 10:15:00
 * @copyright Copyright (c) 2014-2023, Company Genitop. Co., Ltd.
 */
#ifndef _GT5SLAD3BFA_H_
#define _GT5SLAD3BFA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* include --------------------------------------------------------------*/



/* define ---------------------------------------------------------------*/



/* typedef --------------------------------------------------------------*/



/* macros ---------------------------------------------------------------*/



/* class ----------------------------------------------------------------*/



/* global functions / API interface -------------------------------------*/

/* ---------------------------------------------------------------------------------------------------- *\
 * @brief step 1 ：Reference routine, implement the following functions.
 *                 参考例程，实现以下函数
 * ---------------------------------------------------------------------------------------------------- */
/* 外部函数声明 需要用户根据使用的mcu实现，参考文件中有具体的参考代码*/
/**
 * @brief 发送读取函数
 *
 * @param sendbuf 发送数据的buff
 * @param sendlen 发送数据长度
 * @param receivebuf 读取数据的buff
 * @param receivelen 读取数据长度
 */
extern unsigned char gt_read_data(unsigned char *sendbuf, unsigned char sendlen, unsigned char *receivebuf, unsigned int receivelen);


/* ---------------------------------------------------------------------------------------------------- *\
 * @brief step 2 ：Initialization 初始化
 * ---------------------------------------------------------------------------------------------------- */
/**
 * @brief 字库初始化
 *          Follow system initialization and place before font library calls.
 *          随系统初始化，置于spi初始化成功之后，字库调用之前
 * @return Initialization Result : if (0 >= ret_val) fail, else success.
 *          初始化结果：如果 (0 >= ret_val) 失败；其余值为成功
 */
int GT_Font_Init(void);


/* ---------------------------------------------------------------------------------------------------- *\
 * @brief step 3 ：Functions for font libraries. 字库调用函数
 * ---------------------------------------------------------------------------------------------------- */

/********************* 矢量公用部分 *********************/
//中文
#define VEC_SONG_STY         1        //宋体
#define VEC_FANG_STY         2        //仿宋
#define VEC_BLACK_STY        3        //黑体
#define VEC_KAI_STY          4        //楷体

//ASCII码
#define VEC_FT_ASCII_STY     5        //圆角(方头)
#define VEC_DZ_ASCII_STY     6        //打字
#define VEC_CH_ASCII_STY     7        //长黑
#define VEC_BX_ASCII_STY     8        //白斜
#define VEC_BZ_ASCII_STY     9        //线型(白正)
#define VEC_FX_ASCII_STY     10       //方斜
#define VEC_GD_ASCII_STY     11       //歌德
#define VEC_HZ_ASCII_STY     12       //黑正
#define VEC_MS_ASCII_STY     13       //美术
#define VEC_SX_ASCII_STY     14       //手写
#define VEC_ZY_ASCII_STY     15       //正圆
#define VEC_TM_ASCII_STY     16       //时钟体
//拉丁文
#define VEC_YJ_LATIN_STY     17

/******************* 两种调用模式配置 *******************/

/**
 * 方式一 VEC_ST_MODE : 通过使用声明VECFONT_ST结构体变量, 配置结构体信息,
 *                      获取点阵数据到zk_buffer[]数组中.
 * 方式二 VEC_PARM_MODE : 通过指定参数进行调用, 获取点阵数据到pBits[]数组中.
 * ps: 两种方式可同时配置使用, 择一使用亦可.
*/
//#define VEC_ST_MODE
#define VEC_PARM_MODE

/********************* 分割线 *********************/

#ifdef VEC_ST_MODE

    #define ZK_BUFFER_LEN   4608    //可修改大小, 约等于 字号*字号/8.

    typedef struct vecFont
    {
        unsigned long fontCode;     //字符编码中文: GB18030, ASCII/外文: unicode
        unsigned char type;         //字体    @矢量公用部分
        unsigned char size;         //文字大小
        unsigned char thick;        //文字粗细
        unsigned char zkBuffer[ZK_BUFFER_LEN];    //数据存储
    }VECFONT_ST;

    unsigned int get_font_st(VECFONT_ST *font_st);
#endif

#ifdef VEC_PARM_MODE
   /**
    * @brief 矢量文字读取函数
    * @param pBits    文字数据缓存
    * @param sty      文字字体选择  @矢量公用部分
    * @param fontCode 字符编码中文: GB18030, ASCII/外文: unicode
    * @param width  文字宽度
    * @param height 文字高度
    * @param thick  文字粗细
    * @return unsigned int 文字显示宽度
    */
    unsigned int get_font(unsigned char *pBits, unsigned char sty, unsigned long fontCode, unsigned char width, unsigned char height, unsigned char thick);
#endif
/********************* 矢量区域结束 *********************/
/**
 * @brief 灰度矢量文字读取函数
 * @param pBits    文字数据缓存
 * @param sty      文字字体选择  @矢量公用部分
 * @param fontCode 字符编码中文: GB18030, ASCII/外文: unicode
 * @param fontSize 文字大小
 * @param thick    文字粗细
 * @return unsigned int * ret_buff[0] 字符的显示宽度 , ret_buff[1] 字符的灰度阶级[1阶/2阶/3阶/4阶]
 */
unsigned int *get_Font_Gray(unsigned char *pBits, unsigned char sty, unsigned long fontCode, unsigned char fontSize, unsigned char thick);

//转码表
/**
 * @brief unicode编码 转 GBK编码
 * @param unicode 中文的unicode编码
 * @return GBK 编码
 */
unsigned int U2G(unsigned int unicode);

/**
 * @brief BIG5编码 转 GBK编码
 * @param h BIG5编码 高字节
 * @param l BIG5编码 低字节
 * @return GBK 编码
 */
unsigned int BIG52GBK(unsigned char h, unsigned char l);


//字符效果
/*----------------------------------------------------------------------------------------
 * 灰度数据转换函数 2阶灰度/4阶灰度
 * 说明 : 将点阵数据转换为灰度数据 [eg:32点阵数据转2阶灰度数据则转为16点阵灰度数据]
 * 参数 ：
 *   OutPutData灰度数据;     width 宽度; High 高度; grade 灰度阶级[1阶/2阶/3阶/4阶]
 *------------------------------------------------------------------------------------------*/
void Gray_Process(unsigned char *OutPutData, int width, int High, unsigned char Grade);

/*----------------------------------------------------------------------------------------
 * 灰度文字颜色设置
 * BmpDst 目标图片数据
 * BmpSrc 图标图片数据
 * WORD x, WORD y, 图标在目标图片的 (x,y) 位置。
 * WORD src_w, WORD src_h,  图标的宽度和高度
 * WORD dst_w, WORD dst_h,  目标图片的宽度和高度
 * SrcGray 灰度文字数据
 * Grade    灰度阶级[2阶/4阶]
 *------------------------------------------------------------------------------------------*/
void AlphaBlend_whiteBC(unsigned char *BmpDst, unsigned char *BmpSrc, int x, int y,
        int src_w, int src_h, int dst_w, int dst_h, unsigned char *SrcGray, unsigned char Grade);

/*----------------------------------------------------------------------------------------
 * 灰度文字与背景混合
 * BmpDst 目标图片数据
 * BmpSrc 图标图片数据
 * WORD x, WORD y, 图标在目标图片的 (x,y) 位置。
 * WORD src_w, WORD src_h,  图标的宽度和高度
 * WORD dst_w, WORD dst_h,  目标图片的宽度和高度
 * SrcGray 灰度文字数据
 * Grade    灰度阶级[2阶/4阶]
 *------------------------------------------------------------------------------------------*/
void AlphaBlend_blackBC(unsigned char *BmpDst, unsigned char *BmpSrc, int x, int y,
        int src_w, int src_h, int dst_w, int dst_h, unsigned char *SrcGray, unsigned char Grade);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif //!_GT5SLAD3BFA_H_

/*--------------------------------------- end of file ---------------------------------------------*/

