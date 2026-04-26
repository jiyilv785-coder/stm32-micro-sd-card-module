#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)//初始化按键
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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;//启用1号和11号端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

uint8_t Key_GetNum(void)//读取按键操作值
{
	uint8_t KeyNum=0;
	//uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//读取输入寄存器某一个端口的输入值，返回值是uint8
	//uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);//读取整个输入数据寄存器，返回值是uint16，是十六位的数据，每一位代表一个端口值
	//uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//读取输出寄存器的某一位的值，看看输出的是什么
	//uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);//读取整个输出寄存器，返回十六位值
	//以上为端口读取函数
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
		{
			Delay_ms(20);//消除按键按下抖动
			while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
			Delay_ms(20);//消除按键松手抖动
			KeyNum = 1;
		}
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0)
		{
			Delay_ms(20);//消除按键按下抖动
			while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0)
			Delay_ms(20);//消除按键松手抖动
			KeyNum = 2;
		}
	
	return KeyNum;
}
