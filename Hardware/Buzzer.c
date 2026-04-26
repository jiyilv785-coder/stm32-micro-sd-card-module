#include "stm32f10x.h"                  // Device header

void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//配置端口
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//GPIO_Mode_决定了你要调用的gpio口的属性
	//IPU用于读按键,按键接GND松手为高
	//IPD用于读光耦，开漏输出传感器
	//AIN用于ADC采样，输入类型ADC/DAC模拟通道
	//Out_PP推挽输出，驱动LED,继电器
	//Out_OD开漏输出，I²C端口，线与即并联输出
	//AF_OD复用开漏输出
	//AF_PP复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//启用12号端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}
void Buzzer_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void Buzzer_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

void Buzzer_Turn(void)//输出翻转的函数
{
	if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	}
}
