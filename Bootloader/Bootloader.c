#include "Bootloader.h"
#include "bsp_crc.h"
#include "bsp_usart.h"

#include "lcd_console.h"
#include "OSPI_W25Qxx.h"
#include "debug_led.h"
#include "buzzer.h"

#include <stddef.h>
#include <stdint.h>

boot_info_t boot_info;


void factory_mode(uint8_t cmd);
void application_update(void);
void bootloader_cb(void);
void Soft_Jump(uint32_t address);
void Soft_Reboot(void);
boot_status_t boot_info_check(boot_info_t *boot_info_buff);
boot_status_t boot_info_write(void);
boot_status_t boot_info_read(void);
boot_status_t bootloader_flash_init(void);


boot_status_t bootloader_init(void)
{
    boot_status_t boot_status = BOOT_OK;
    boot_status = boot_info_read();         //读取bootloader控制字段信息
    if(boot_status != BOOT_OK) return boot_status; 
	
	return boot_status;
}

void bootloader_check(void)
{

    boot_status_t boot_status = BOOT_OK;
	
    boot_status = boot_info_check(&boot_info);

	switch(boot_status)
	{
        case BOOT_APP_READY:        //上一次启动到APP失败
        case BOOT_FACTORY_READY:    //上一次进入工厂模式失败
            //Soft_Jump(boot_info.boot_addr);
            if(boot_info.boot_error_count >=3 )
            {
//                console_printf("\033[30;31mToo many boot errors! \nPlease contact the after-sales service phone number.\033[30;37m\n");
                boot_info.boot_status = BOOT_ERROR;
			}
			else
			{
				// 重启尝试三次
//				console_printf("\033[30;31mLast boot failed. Number of retries: %d. rebooting\033[30;37m\n", boot_info.boot_error_count);
			}
			boot_info.boot_error_count ++;
			boot_info_write();
			HAL_Delay(2000);
			Soft_Reboot();
            break;

//        case BOOT_APP_ERROR:        //上一次启动到APP失败超过三次，进入错误状态
//        case BOOT_FAC_ERROR:        //上一次进入工厂模式失败超过三次，进入错误状态

//            break;

        case BOOT_INFO_READ_ERROR:       //启动信息错误，可能是第一次启动或者启动信息被破坏，进入工厂模式
//            console_printf("\033[30;31mBoot information error detected! Please contact the after-sales service phone number.\033[30;37m\n");
            factory_mode(BOOT_INFO_READ_ERROR);
            break;
		
        case BOOT_APP_CHECK_ERROR:  //应用程序校验错误，进入工厂模式
//            console_printf("\033[30;31mApplication check error detected! Please contact the after-sales service phone number.\033[30;37m\n");
            break;
		
        case BOOT_APP_UPDATE_ERROR:  //应用程序更新错误，进入工厂模式
//            console_printf("\033[30;31mApplication update error detected! Please contact the after-sales service phone number.\033[30;37m\n");
            break;
		
        case BOOT_ERROR:              //严重启动错误，进入工厂模式
            factory_mode(boot_status);
            break;
        case BOOT_OK:                 //正常启动
//           console_printf("bootloader init ok!,loading firmware \n");
            boot_info.boot_status = BOOT_APP_READY;
            boot_info.boot_count++;
            boot_info_write();
//            console_clear_screen();
            HAL_Delay(2000);
    //		OSPI_W25Qxx_MemoryMappedMode();
    //		Soft_Jump(boot_info.boot_addr);
            break;
        default:
//            console_printf("\033[30;31mUnknown boot status! Please contact the after-sales service phone number.\033[30;37m\n");
			while(1);
	}
}




boot_status_t boot_info_write(void)
{
    boot_info_t boot_info_temp;

    boot_info.flash_page_index ++;  //切换页地址
    if(boot_info.flash_page_index > BOOT_INFO_MAX_ENTRIES)   //如果页地址超过最大值，重置为1
    {
        boot_info.flash_page_index = 1;
        OSPI_W25Qxx_SectorErase(0);
    }

    boot_info.crc16 = hardware_crc16_calculate((uint8_t *)&boot_info,offsetof(boot_info_t, crc16));
    OSPI_W25Qxx_WriteBuffer((uint8_t *)&boot_info, BOOT_INFO_ADDR + ((boot_info.flash_page_index -1) * BOOT_INFO_TYPE_SIZE), BOOT_INFO_TYPE_SIZE);  //循环写

    OSPI_W25Qxx_ReadBuffer((uint8_t *)&boot_info_temp, BOOT_INFO_ADDR + ((boot_info.flash_page_index -1) * BOOT_INFO_TYPE_SIZE), BOOT_INFO_TYPE_SIZE);
    for(uint8_t i = 0; i < BOOT_INFO_TYPE_SIZE; i++)
    {
        if(((uint8_t *)&boot_info)[i] != ((uint8_t *)&boot_info_temp)[i])
        {
//            console_printf("\033[30;31mBoot info write error! \033[30;37m\n");
            return BOOT_FLASH_INFO_ERROR;
        }
    }
    return BOOT_OK;
}

boot_status_t boot_info_read(void)
{
    uint16_t max_index = 0;      //记录当前最大页地址，有效页地址从1开始，到4095/BOOT_INFO_TYPE_SIZE - 1结束
    boot_info_t boot_info_temp;

    for(uint16_t page_addr = 0; page_addr < BOOT_INFO_MAX_ENTRIES + 1; page_addr++)
    {
        OSPI_W25Qxx_ReadBuffer((uint8_t *)&boot_info_temp, BOOT_INFO_ADDR + (page_addr * BOOT_INFO_TYPE_SIZE), BOOT_INFO_TYPE_SIZE);
        
        if(boot_info_temp.crc16 == hardware_crc16_calculate((uint8_t *)&boot_info_temp,offsetof(boot_info_t, crc16)))
        {
            if(boot_info_temp.flash_page_index > max_index)      //找到更大的页地址，更新最大页地址
            {
                max_index = boot_info_temp.flash_page_index;     //更新最大页地址
            }
        }
    }
    if(max_index == 0 || max_index == 0xff )   //没有找到有效的启动信息
    {
        return BOOT_FLASH_INFO_ERROR;
    }
    else
    {
        OSPI_W25Qxx_ReadBuffer((uint8_t *)&boot_info, BOOT_INFO_ADDR + ((max_index -1) * BOOT_INFO_TYPE_SIZE), BOOT_INFO_TYPE_SIZE);
        return BOOT_OK;
    }
}


boot_status_t boot_info_check(boot_info_t *boot_info_buff)
{

    if(boot_info_buff->boot_status != BOOT_OK)  //启动状态
    {
        return boot_info_buff->boot_status;
    }

	//固件校验CRC校验
//	if()
//	{
//	
//	}
	//固件签名校验
	return BOOT_OK;
}

void app_check_up(uint32_t app_static_addr)
{
//    OSPI_W25Qxx_ReadBuffer((uint8_t *)&app_static_addr,APP_OFFSET_ADDR,sizeof(uint32_t));
	if((app_static_addr & 0x2FFE0000) != 0x20000000)
	{
//		console_printf("\033[30;31mapplication check error!\033[30;37m\n");
//		return BOOT_APP_CHECK_ERROR;
	}
}

void factory_mode(uint8_t cmd)
{
	debug_led_set(200,200);
	switch(cmd) 
    {
        case BOOT_FLASH_INFO_ERROR:
			//先调试，后期添加通化，等待下发初始化命令
            if(bootloader_flash_init() != BOOT_OK)
            {
//                console_printf("\033[30;31mBoot info init error!\n");
                
            }
            else 
            {
//                console_printf("\033[30;32mBoot info init ok! Please restart the device.\n\033[30;37m");
            }
        break;
			
        case BOOT_INFO_READ_ERROR:
   
            while(1);   //后期添加通讯功能
		break;
		
        default:
            //Error_Handler();
            while(1);
        break;
    }
	
    while (1);
	Soft_Reboot();
}

boot_status_t bootloader_flash_init(void)
{
	boot_info_t boot_info_cb;
    if(OSPI_W25Qxx_Init() != OSPI_W25Qxx_OK)
    {
//        console_printf("\033[30;31mFlash init error! Please contact the after-sales service phone number.\033[30;37m\n");
    }
    else
    {
//        console_printf("\033[30;33mFlash init ok!\033[30;37m\n");
    }
    if(OSPI_W25Qxx_SectorErase(0) != OSPI_W25Qxx_OK)
    {
//        console_printf("\033[30;31mFlash erase error! Please contact the after-sales service phone number.\033[30;37m\n");
    }
    else
    {
//        console_printf("\033[30;33mFlash Chip Eras ok!\033[30;37m\n");
    }
    boot_info.flash_page_index = 1;
    boot_info.boot_status = BOOT_FACTORY_READY;
	boot_info.boot_addr = BOOT_APP_ADDR_EXA;
	boot_info.boot_error_count = 1;
	boot_info.boot_count = 0;
	boot_info.mcu_id = HAL_GetUIDw0(); // 获取MCU唯一ID
	boot_info.crc16 = hardware_crc16_calculate((uint8_t *)&boot_info,offsetof(boot_info_t, crc16));
    if(OSPI_W25Qxx_WriteBuffer((uint8_t *)&boot_info, BOOT_INFO_ADDR, BOOT_INFO_TYPE_SIZE) != OSPI_W25Qxx_OK)
    {
//        console_printf("\033[30;31mFlash write error! \033[30;37m\n");
    }
    	//回读检查是否写入成功
	OSPI_W25Qxx_ReadBuffer((uint8_t *)&boot_info_cb,BOOT_INFO_ADDR,BOOT_INFO_TYPE_SIZE);
	
	for(uint8_t i = 0;i < BOOT_INFO_TYPE_SIZE; i++)
    {
        if(((uint8_t *)&boot_info)[i] != ((uint8_t *)&boot_info_cb)[i])
        {
//            console_printf("\033[30;31mFlash write error!\033[30;37m\n");
            return BOOT_FLASH_INFO_ERROR;
        }
    }
//	console_printf("\033[30;32mboot info config ok!\033[30;37m\n");
    return BOOT_OK;
}



void application_update(void)
{
    
}


void Soft_Jump(uint32_t address)
{
    typedef void (*pFunction)(void);

    pFunction Jump_To_Application;

    OSPI_W25Qxx_MemoryMappedMode(); // 将外部闪存映射到系统地址空间

    /*Disable IRQ Interrupts*/
	for (uint8_t i = 0; i < 8; i++) 
	{
		NVIC->ICER[i] = 0xFFFFFFFF;
	}
    __disable_irq();

    // Set stack pointer as in application's vector table
    __set_MSP(*((volatile uint32_t*)address));

    // get jump address from application vector table
    // load this address into function pointer
    Jump_To_Application = (pFunction)(*(volatile uint32_t*)(address + 4));

    // Go
    Jump_To_Application();
}

void Soft_Reboot(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

