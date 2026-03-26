#ifndef _BSP_IWDG_H_
#define _BSP_IWDG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

extern IWDG_HandleTypeDef hiwdg1;

void MX_IWDG1_Init(void);

#ifdef __cplusplus
}
#endif
#endif

