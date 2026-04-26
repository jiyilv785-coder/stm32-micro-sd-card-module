#include "sd_card.h"
#include "SPI.h"
#include "usart.h"
#include "Delay.h"
#include <string.h>

extern void SendStr(char* s);
extern void SendNum(int num);

#define CMD0    0   
#define CMD8    8   
#define CMD16   16  
#define CMD17   17  
#define CMD24   24  
#define CMD41   41  
#define CMD55   55  
#define CMD58   58  



static struct {
    uint8_t initialized;  
    uint8_t card_type;    
} sd_state = {0, 0};





static uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg)
{
     uint8_t response = 0xFF;
    
    // 确保CS为高
    LVSPI_W_CS(1);
    for(int i=0; i<8; i++) LVSPI_SwapByte(0xFF);
    
    // 拉低CS开始命令
    LVSPI_W_CS(0);
    
    // 发送命令前等待
    LVSPI_SwapByte(0xFF);
    
    // 发送命令
    LVSPI_SwapByte(0x40 | cmd);
    LVSPI_SwapByte(arg >> 24);
    LVSPI_SwapByte(arg >> 16);
    LVSPI_SwapByte(arg >> 8);
    LVSPI_SwapByte(arg);
    
    // 发送CRC
    if(cmd == CMD0) {
        LVSPI_SwapByte(0x95);
    } else if(cmd == CMD8) {
        LVSPI_SwapByte(0x87);
    } else {
        LVSPI_SwapByte(0xFF);
    }
    
    // 等待响应 - 增加超时保护
    uint16_t timeout = 1000;  // 增加超时计数
    do {
        response = LVSPI_SwapByte(0xFF);
        timeout--;
        if(timeout == 0) {
            // 超时，结束命令
            LVSPI_W_CS(1);
            LVSPI_SwapByte(0xFF);
            return 0xFF;  // 返回超时标记
        }
    } while(response == 0xFF);
    
    // 结束命令
    LVSPI_W_CS(1);
    LVSPI_SwapByte(0xFF);
    
    return response;
}



static SD_Error SD_CardInit(void)
{
    uint8_t response;
    uint16_t timeout;
    
    
    // 1.
    for(volatile int i=0; i<100000; i++);
    
    // 2. 发送至少74个时钟脉冲（CS高电平）
    LVSPI_W_CS(1);
    for(int i=0; i<100; i++) LVSPI_SwapByte(0xFF);
    
    // 3. CMD0- 复位SD卡
    timeout = 0xFFF;
    do{
        response = SD_SendCommand(CMD0, 0);
        if(timeout-- == 0) return SD_ERROR;
    }while(response != 0x01);
    
    // 4. CMD8检查卡版本
    response = SD_SendCommand(CMD8, 0x1AA);
    
    
    if(response == 0x05) 
    {
        sd_state.card_type = 0;  // V1.x卡
       
    }
    else if(response == 0x01)
    {
        sd_state.card_type = 1;  // V2+卡
        
        for(int i=0; i<4; i++) LVSPI_SwapByte(0xFF);  // 读取额外响应
    }
    else 
    {
        return SD_ERROR;
    }
    
    // 5. ACMD41
    
    int retry_count = 0;
    uint8_t r41 = 0x01;  
    
    while(retry_count < 5 && r41 == 0x01)  
    {
        
        
        // CMD55
        uint8_t r55 = SD_SendCommand(CMD55, 0);
        
        
        r41 = SD_SendCommand(CMD41, 0x40000000);
        
        
        
        if(r41 == 0x00) break;  
        
        Delay_ms(100);  
        retry_count++;
    }
    
     if(r41 != 0x00) {
        return                                                                                                                                   SD_ERROR;
    }
    
    Delay_ms(100);
    
    // 6. CMD58读取OCR
    response = SD_SendCommand(CMD58, 0);
    if(response != 0x00) return SD_ERROR;
    for(int i=0; i<4; i++) LVSPI_SwapByte(0xFF);
    Delay_ms(10);
	
    // 7. CMD16设置块大小
    response = SD_SendCommand(CMD16, 512);
    if(response != 0x00) return SD_ERROR;
    
   //不转为高速
    LVSPI_SetSpeed(SPI_BaudRatePrescaler_256); 
    
    sd_state.initialized = 1;
    return SD_OK;
}





SD_Error SD_Init(void)
{
    if(sd_state.initialized) {
        return SD_OK;
    }
    return SD_CardInit();
}



SD_Error SD_ReadSector(uint32_t sector, uint8_t* buffer, uint32_t count)
{
    uint8_t response;
    uint32_t timeout;
    
    if(!sd_state.initialized) return SD_ERROR;
    if(count != 1) return SD_ERROR;
    
    uint32_t address;
    if(sd_state.card_type == 0) {
        address = sector * 512;
    } else {
        address = sector;
    }
    
    LVSPI_W_CS(1);
    LVSPI_SwapByte(0xFF);
    
    LVSPI_W_CS(0);
    LVSPI_SwapByte(0x40 | CMD17);
    LVSPI_SwapByte(address >> 24);
    LVSPI_SwapByte(address >> 16);
    LVSPI_SwapByte(address >> 8);
    LVSPI_SwapByte(address);
    LVSPI_SwapByte(0xFF);
    
    timeout = 10000;
    do {
        response = LVSPI_SwapByte(0xFF);
        timeout--;
        if(timeout == 0) {
            LVSPI_W_CS(1);
            return SD_ERROR;
        }
    } while((response & 0x80) && timeout);
    
    if(response != 0x00) {
        LVSPI_W_CS(1);
        return SD_ERROR;
    }
    
    // 等待数据令牌
    timeout = 1000000;
    uint8_t token;
    do {
        token = LVSPI_SwapByte(0xFF);
        timeout--;
        if(timeout == 0) {
            LVSPI_W_CS(1);
            return SD_ERROR;
        }
    } while((token == 0xFF) && timeout);
    
    if(token != 0xFE) {
        LVSPI_W_CS(1);
        return SD_ERROR;
    }
    
    for(uint16_t i=0; i<512; i++) {
        buffer[i] = LVSPI_SwapByte(0xFF);
    }
    
    LVSPI_SwapByte(0xFF); // CRC 丢弃
    LVSPI_SwapByte(0xFF);
    
    LVSPI_W_CS(1);
    LVSPI_SwapByte(0xFF);
    
    return SD_OK;
}



//// 简单验证写入是否成功
//void SD_WriteTest(void)
//{
//    uint8_t test_data[512];
//    uint8_t read_back[512];
//    
//    SendStr("\r\n=== SD Write Test ===\r\n");
//    
//    // 准备测试数据
//    memset(test_data, 0xAA, 512);
//    test_data[0] = 0xDE;
//    test_data[1] = 0xAD;
//    test_data[2] = 0xBE;
//    test_data[3] = 0xEF;
//    
//    // 写入一个测试扇区（避开重要区域）
//    uint32_t test_sector = 65568;  // 选择一个靠后的扇区
//    
//    if(SD_WriteSector(test_sector, test_data, 1) == SD_OK)
//    {
//        SendStr("Write OK. Now reading back...\r\n");
//        
//        Delay_ms(100);  // 等待写入完成
//        
//        if(SD_ReadSector(test_sector, read_back, 1) == SD_OK)
//        {
//            // 比较数据
//            if(memcmp(test_data, read_back, 512) == 0)
//            {
//                SendStr("SUCCESS: Write verification passed!\r\n");
//            }
//            else
//            {
//                SendStr("ERROR: Data mismatch!\r\n");
//            }
//        }
//        else
//        {
//            SendStr("ERROR: Failed to read back!\r\n");
//        }
//    }
//    else
//    {
//        SendStr("ERROR: Write failed!\r\n");
//    }
//}





SD_Error SD_WriteSector(uint32_t sector, const uint8_t* buffer, uint32_t count)
{
    uint8_t response;
    uint32_t timeout;
    
    if(!sd_state.initialized) return SD_ERROR;
    if(count != 1) return SD_ERROR;
    
    uint32_t address;
    if(sd_state.card_type == 0) {
        address = sector * 512;
    } else {
        address = sector;
    }
    
    LVSPI_W_CS(1);
    LVSPI_SwapByte(0xFF);
    Delay_us(10);
    
    LVSPI_W_CS(0);
    Delay_us(10);
    
    LVSPI_SwapByte(0x40 | 24);
    LVSPI_SwapByte(address >> 24);
    LVSPI_SwapByte(address >> 16);
    LVSPI_SwapByte(address >> 8);
    LVSPI_SwapByte(address);
    LVSPI_SwapByte(0xFF);
    
    timeout = 10000;
    do {
        response = LVSPI_SwapByte(0xFF);
        timeout--;
    } while((response & 0x80) && timeout);
    
    if(response != 0x00) {
        LVSPI_W_CS(1);
        LVSPI_SwapByte(0xFF);
        return SD_ERROR;
    }
    
    LVSPI_SwapByte(0xFE);  // 数据令牌
    
    for(uint16_t i=0; i<512; i++) {
        LVSPI_SwapByte(buffer[i]);
    }
    
    LVSPI_SwapByte(0x00); // CRC
    LVSPI_SwapByte(0x00);
    
    uint8_t data_response = 0xFF;
    for(int retry = 0; retry < 20; retry++) {
        data_response = LVSPI_SwapByte(0xFF);
        if(data_response != 0xFF) break;
    }
    
    if((data_response & 0x1F) != 0x05) {
        LVSPI_W_CS(1);
        LVSPI_SwapByte(0xFF);
        return SD_ERROR;
    }
    
    timeout = 500000;
    do {
        response = LVSPI_SwapByte(0xFF);
        timeout--;
    } while(response == 0x00 && timeout);
    
    LVSPI_W_CS(1);
    LVSPI_SwapByte(0xFF);
    
    return SD_OK;
}








uint32_t SD_GetSectorCount(void)
{
    return 60773376;  
}










// 在sd_card.c中添加验证函数
//void VerifySectorRead(uint32_t sector)
//{
//    uint8_t buffer[512];
//    
//    SendStr("\nVerifying sector ");
//    SendNum(sector);
//    SendStr(":\n");
//    
//    if(SD_ReadSector(sector, buffer, 1) == SD_OK)
//    {
//        // 显示前64字节
//        SendStr("First 64 bytes:\n");
//        for(int i = 0; i < 64; i++)
//        {
//            SendHex(buffer[i]);
//            SendStr(" ");
//            if((i+1) % 16 == 0) SendStr("\n");
//        }
//        
//        // 检查引导扇区签名
//        SendStr("\nBoot signature at 510-511: ");
//        SendHex(buffer[510]);
//        SendStr(" ");
//        SendHex(buffer[511]);
//        SendStr("\n");
//    }
//    else
//    {
//        SendStr("Failed to read sector!\n");
//    }
//}



