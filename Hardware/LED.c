//驱动程序的主体代码
#include "stm32f10x.h"                  // Device header
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//该处时钟控制也可以按位或的操作选中ABC的IO口
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//尾部参数PP或者OD是关键
	//GPIO_Mode_Out_PP里面PP代表PUSH_PULL推挽输出，改为OD则是输出开漏模式OPEN_DRAIN
	//PP高低电平都有驱动能力，OD仅有低电平有驱动能力，高电平时只能依靠上拉电阻到VDD
	//PP用于SPI,UART,通用高低电平，不可以多机并联容易短路
	//OD的上拉电阻可以拉到任意电压用于I²C，5V器件通信，可以多机并联
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//此处可选中X端口所有引脚，写入GPIO_Pin_All
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//以上为固定操作无需担心过于复杂

	GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);
}
void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void LED1_Turn(void)//输出翻转的函数
{
	if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	}
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
}

void LED2_Turn(void)//输出翻转的函数
{
	if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2) == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	}
}
