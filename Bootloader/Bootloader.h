#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include "main.h"
#include <stdint.h>

#define BOOT_INFO_ADDR          0x00000000      //启动状态校验存放地址
#define APP_OFFSET_ADDR		    0x1000
#define BOOT_FLASH_SECTOR_SIZE	0x1000          //Flash扇区大小为4KB
#define BOOT_INFO_TYPE_SIZE     sizeof(boot_info_t) //启动信息结构体大小
#define BOOT_INFO_MAX_ENTRIES   (BOOT_FLASH_SECTOR_SIZE / BOOT_INFO_TYPE_SIZE - 1)       //每个扇区可以存储的启动信息条目数


#define BOOTLOADER_MCU_INFO "STM32H7B0VET6"
#define BOOTLOADER_VERSION  "V1.00ce"

// 工厂模式标志
typedef enum {

    BOOT_APP_READY          = 0x10,       // 准备进入应用程序
    BOOT_FACTORY_READY      = 0x11,       // 准备进入工厂模式

    BOOT_OK                 = 0xF0,       // 正常启动
    BOOT_APP_ERROR			= 0xF1,       // 启动到APP错误
	BOOT_FAC_ERROR			= 0xF2,		  // 启动到工厂模式错误
    BOOT_INFO_READ_ERROR    = 0xF3,       // 启动信息错误
    BOOT_APP_CHECK_ERROR    = 0xF4,       // 应用程序校验错误
    BOOT_FLASH_INFO_ERROR   = 0xF5,       // Flash 擦写平衡信息错误
    BOOT_ERROR              = 0xFE,       // 严重启动错误
    BOOT_APP_UPDATE_ERROR   = 0xFF,       // 其他错误状态
}boot_status_t;

typedef enum {
	BOOT_APP_ADDR_EXA = 0x00001000,
	BOOT_APP_ADDR_EXB = 0x0000F000,
	BOOT_APP_ADDR_LOA = 0x08008000,
	BOOT_APP_ADDR_LOB = 0x08004000,
}boot_addr_t;

#pragma pack(push, 1)
typedef struct{
	char boot_mcu_info[16];
    char bootloader_version[8];
	char build_date[20];
    char build_time[20];
}fameware_info_t;
#pragma pack(pop)

// 使用section属性将变量放置在指定地址
static fameware_info_t fameware_info __attribute__((used,section(".boot_info")))  = {
   .boot_mcu_info          = BOOTLOADER_MCU_INFO,
   .bootloader_version     = BOOTLOADER_VERSION,
   .build_date  = __DATE__,
   .build_time  = __TIME__,
};


#pragma pack(push, 1)
typedef struct {
    uint8_t     flash_page_index;    //页地址标志,每次写入boot_info时切换页地址，保证每次写入都在新的页地址上，增加Flash寿命
    uint8_t     boot_status;        //记录启动状态 上一次启动失败/上一次启动成功/从A分区启动/从B分区启动
    uint8_t     boot_error_count;   // 启动错误次数
    uint32_t    boot_addr;          // 启动地址，bootloader根据该地址进行跳转
	uint32_t    boot_count;         // 启动次数
    uint32_t    mcu_id;             // MCU ID
    uint16_t    crc16;              // CRC16校验
}boot_info_t;

extern boot_info_t boot_info;

typedef struct {
    uint8_t  signature[64];   // 签名
    uint8_t  public_key[64];  // 公钥
    uint32_t firmware_crc;    // 固件CRC32
    uint32_t total_size;      // 固件总大小
    uint32_t timestamp;       // 时间戳
} SecurityInfo_t;
#pragma pack(pop)


boot_status_t bootloader_init(void);

#endif