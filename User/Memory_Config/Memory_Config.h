#ifndef _MEMORY_CONFIG_H_
#define _MEMORY_CONFIG_H_

#include "main.h"

// 定义绝对地址变量宏

// 1. 辅助宏：将地址转换为段名
#define ADDR_TO_SECTION(addr) ".ARM.__at_"#addr

// 2. 绝对地址宏
#ifndef ABSOLUTE_ADDRESS_VAR
#define ABSOLUTE_ADDRESS_VAR(type, name, addr) \
    type name __attribute__((used, section(ADDR_TO_SECTION(addr))))
#endif

#define	FRAME_BUFFER0_ADDR	0x24000000
#define	FRAME_BUFFER1_ADDR	0x24006600
#define FRAME_BUFFER0_SIZE	26112

ABSOLUTE_ADDRESS_VAR(uint8_t,LCD_FRAME_BUFFER0[FRAME_BUFFER0_SIZE],FRAME_BUFFER0_ADDR);
ABSOLUTE_ADDRESS_VAR(uint8_t,LCD_FRAME_BUFFER1[FRAME_BUFFER0_SIZE],FRAME_BUFFER1_ADDR);

#endif

