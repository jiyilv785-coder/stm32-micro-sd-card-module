#ifndef __SD_CARD_H
#define __SD_CARD_H

#include "stm32f10x.h"

// SD卡返回值定义
typedef enum {
    SD_OK = 0,
    SD_ERROR = 1,
    SD_TIMEOUT = 2
} SD_Error;

// 初始化函数
SD_Error SD_Init(void);

// 去初始化（用于热插拔）
void SD_Deinit(void);

// 读扇区
SD_Error SD_ReadSector(uint32_t sector, uint8_t* buffer, uint32_t count);

// 写扇区
SD_Error SD_WriteSector(uint32_t sector, const uint8_t* buffer, uint32_t count);

// 获取容量
uint32_t SD_GetSectorCount(void);


void VerifySectorRead(uint32_t sector);//验【证读取的数据
void SD_WriteTest(void);

#endif

