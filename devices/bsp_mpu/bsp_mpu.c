#include "bsp_mpu.h"


/*
* 配置MPU内存区域
* baseaddr: 基地址
* regionnum: 设置保护区 (0-7)
* size: 区域大小 (参见CMSIS定义的MPU_REGION_SIZE_xxx)
* accessperm: 访问权限 (参见CMSIS定义的MPU_REGION_xxx_ACCESS_xxx)
* isshareable: 是否允许共用 (1: 是, 0: 否)
* iscacheable: 是否允许cache (1: 是, 0: 否)
* isbufferable: 是否允许缓冲 (1: 是, 0: 否)
*/
void MPU_MemeoryRemap(uint32_t baseaddr, uint8_t regionnum, uint8_t size, uint8_t accessperm, uint8_t isshareable, uint8_t iscacheable, uint8_t isbufferable)
{
    if((size % 32)||(size == 0))
        return;
    MPU_disable();

    MPU->RNR = regionnum; // 设置保护区域
    MPU->RBAR = baseaddr; // 设置基地址
	
	// 0x00 << 28
	//
    uint32_t rasr_value = ((uint32_t)MPU_INSTRUCTION_ACCESS_ENABLE << MPU_RASR_XN_Pos) |	// 允许指令访问
                          ((uint32_t)accessperm << MPU_RASR_AP_Pos) |						// 访问权限
                          ((uint32_t)MPU_TEX_LEVEL0 << MPU_RASR_TEX_Pos) |              	// TEX级别0
                          ((uint32_t)(isshareable ? MPU_ACCESS_SHAREABLE : MPU_ACCESS_NOT_SHAREABLE) << MPU_RASR_S_Pos) |   	// Shareable属性
                          ((uint32_t)(iscacheable ? MPU_ACCESS_CACHEABLE : MPU_ACCESS_NOT_CACHEABLE) << MPU_RASR_C_Pos) |   	// Cacheable属性
                          ((uint32_t)(isbufferable ? MPU_ACCESS_BUFFERABLE : MPU_ACCESS_NOT_BUFFERABLE) << MPU_RASR_B_Pos) |	// Bufferable属性
                          ((uint32_t)0x00 << MPU_RASR_SRD_Pos) |    // 子区域禁用位，全部启用
                          ((uint32_t)size << MPU_RASR_SIZE_Pos) |   // 区域大小
                          ((uint32_t)MPU_REGION_ENABLE << MPU_RASR_ENABLE_Pos);     // 启用区域
    MPU->RASR = rasr_value; // 设置属性和启用区域
    MPU_enable();
}

void MPU_disable(void)
{
    SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;       // 禁用MemManage 
    MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;          // 禁用MPU
}

void MPU_enable(void)
{
    MPU->CTRL |= MPU_CTRL_ENABLE_Msk | MPU_PRIVILEGED_DEFAULT;   // 启用MPU和特权默认内存映射
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;        // 启用MemManage
}

