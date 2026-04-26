#ifndef __SPI_H
#define __SPI_H
#include "stdint.h"
// 宏定义，方便使用
#define SPI1_SetSpeed LVSPI_SetSpeed
#define SPI1_ReadWriteByte LVSPI_SwapByte

void LVSPI_W_CS(uint8_t BitValue);
void LVSPI_Init(void);
void LVSPI_Start(void);
void LVSPI_Stop(void);
void LVSPI_SetSpeed(uint8_t SPI_BaudRatePrescaler);


uint8_t LVSPI_SwapByte(uint8_t ByteSend);//数据交换高效率版
uint8_t LVSPI_SwapByte1(uint8_t ByteSend);
uint8_t LVSPI_SwapByte2(uint8_t ByteSend);
uint8_t LVSPI_SwapByte3(uint8_t ByteSend);
#endif
