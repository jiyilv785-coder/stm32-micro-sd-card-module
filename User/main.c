#include "stm32f10x.h"
#include "usart.h"
#include "ff.h"
#include "sd_card.h"
#include <string.h>
#include "SPI.h"  
#include "Delay.h"
#include "diskio.h"

#define TEST_SECTOR 0x100000  // 1048576扇区，大约512MB位置



int main(void)
{
    // 1. 硬件初始化
//    USART1_Init(115200);           // 如果需要串口输出，可保留
    LVSPI_Init();
    Delay_ms(100);
    
    // 2. SD卡和文件系统初始化
    
    if(SD_Init() != SD_OK) {
      
        while(1);
    }
    
   
    FATFS fs;
    FRESULT res = f_mount(&fs, "0:", 1);
    if(res != FR_OK) {
        // 可扩展错误码打印
        while(1);
    }
    
    // 3. 创建带日期的TXT文件
    // ===== 在此处修改文件建立日期（文件内容中的日期） =====
    const char *date_string = "2026-04-24 10:00:00";   // 可按需修改
	
	
	
    // =================================================
    
    FIL fil;
    UINT bw;
    res = f_open(&fil, "0:/INFO.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    if(res == FR_OK) {
        char buffer[100];
        int len = sprintf(buffer, "File created on: %s\r\n", date_string);
        f_write(&fil, buffer, len, &bw);
		const char *test_string = "stm32 test create txt document";
		f_write(&fil, test_string, strlen(test_string), &bw);
        f_close(&fil);
    } 
    
    // 4. 卸载文件系统
    f_mount(NULL, "0:", 0);
    
    while(1);
}
