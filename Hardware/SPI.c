#include "stm32f10x.h"                  // Device header
#include "stdint.h"

#define SPI_PORT  GPIOB
#define Pin_CS    GPIO_Pin_12
#define Pin_SCK   GPIO_Pin_13
#define Pin_MOSI  GPIO_Pin_15
#define Pin_MISO  GPIO_Pin_14


//从机选择
void LVSPI_W_CS(uint8_t BitValue)//CS对应PA4引脚
{
	GPIO_WriteBit(SPI_PORT, Pin_CS, (BitAction)BitValue);
}

// SPI速度设置（关键函数）
void LVSPI_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    SPI2->CR1 &= 0xFFC7;  // 清除波特率控制位(bit5:3)
    SPI2->CR1 |= SPI_BaudRatePrescaler;  // 设置新分频
}

void LVSPI_Init(void)//输出引脚为推挽输出，输入引脚为浮空或者上拉输入
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	//输出引脚配置
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = Pin_CS ;
 	GPIO_Init(SPI_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = Pin_SCK | Pin_MOSI;
 	GPIO_Init(SPI_PORT, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = Pin_MISO;
 	GPIO_Init(SPI_PORT, &GPIO_InitStructure);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode                    = SPI_Mode_Master;//设置主机还是从机
	SPI_InitStructure.SPI_Direction               = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize                = SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit                = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_BaudRatePrescaler       = SPI_BaudRatePrescaler_256;//  72MHz/n
	SPI_InitStructure.SPI_CPOL                    = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA	                  = SPI_CPHA_1Edge; 
	SPI_InitStructure.SPI_NSS                     = SPI_NSS_Soft;
	SPI_InitStructure.SPI_CRCPolynomial           = 7; 
	SPI_Init(SPI2, &SPI_InitStructure);
	
	SPI_Cmd(SPI2,ENABLE);
	
	LVSPI_W_CS(1);
}

/*时序代码*/
void LVSPI_Start(void)
{
	LVSPI_W_CS(0);
}


void LVSPI_Stop(void)
{
	LVSPI_W_CS(1);
}

//交换字节模式仅限于0和3，此代码只实现0模式
//uint8_t LVSPI_SwapByte0(uint8_t ByteSend)
//{
//	uint8_t i,ByteReceive = 0x00;
//	
//	for(i=0;i<8;i++)
//	{
//		LVSPI_W_MOSI(ByteSend & (0x80 >> i));
//		LVSPI_W_SCK(1);
//		if(LVSPI_R_MISO() == 1)
//			ByteReceive |= (0x80 >> i);
//		LVSPI_W_SCK(0);
//	}
//	
//	return ByteReceive;
//}


//模式0
uint8_t LVSPI_SwapByte(uint8_t ByteSend)//数据交换高效率版
{
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);//TXE发送缓冲器，标志位为1时缓冲器为空，写入SPI_DR时自动清除标志位
	
	SPI_I2S_SendData(SPI2, ByteSend);//写数据到DR
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);//接收缓冲器RXNE，为1时表明在接收缓冲器中包含有效的接收数据
	
	return SPI_I2S_ReceiveData(SPI2);//完成SPI一个字节的交换
}


////模式1
//uint8_t LVSPI_SwapByte1(uint8_t ByteSend)//数据交换高效率版
//{
//	uint8_t i,ByteReceive = 0x00;
//	
//	for(i=0;i<8;i++)
//	{
//		LVSPI_W_SCK(1);
//		LVSPI_W_MOSI(ByteSend & (0x80>> i));
//		LVSPI_W_SCK(0);
//		if(LVSPI_R_MISO() == 1)
//			ByteReceive |= (0x80 >> i);
//		
//	}
//	
//	return ByteReceive;
//}


////模式2
//uint8_t LVSPI_SwapByte2(uint8_t ByteSend)//数据交换高效率版
//{
//	uint8_t i;
//	
//	for(i=0;i<8;i++)
//	{
//		LVSPI_W_MOSI(ByteSend & 0x80 );
//		ByteSend <<= 1;
//		LVSPI_W_SCK(0);
//		if(LVSPI_R_MISO() == 1)
//			ByteSend |= 0x01;
//		LVSPI_W_SCK(1);
//	}
//	
//	return ByteSend;
//}


////模式3
//uint8_t LVSPI_SwapByte3(uint8_t ByteSend)//数据交换高效率版
//{
//	uint8_t i,ByteReceive = 0x00;
//	
//	for(i=0;i<8;i++)
//	{
//		LVSPI_W_SCK(0);
//		LVSPI_W_MOSI(ByteSend & (0x80>> i));
//		LVSPI_W_SCK(1);
//		if(LVSPI_R_MISO() == 1)
//			ByteReceive |= (0x80 >> i);
//		
//	}
//	
//	return ByteReceive;
//}
