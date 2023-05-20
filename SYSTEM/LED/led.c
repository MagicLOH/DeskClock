#include "led.h"

/*
**************************************************************************************************
    
		函数功能：LED初始化
    硬件连接：
            LED 1 -- PB6 --低电平点亮
            LED 2 -- PB7 --低电平点亮
            LED 3 -- PB8 --低电平点亮
            LED 4 -- PB9 --低电平点亮
						
**************************************************************************************************
*/
void LED_Init(void )
{
    //1.开启时钟
	
    RCC->APB2ENR |=1<<3;
	
    //2.设置P6/7/8/9为通用推挽输出模式 默认低电平
/*	
	//P6
	GPIOB->CRL &=0xF0FFFFFF;
	GPIOB->CRL |=0x03000000;
	//P7
	GPIOB->CRL &=0x0FFFFFFF;
	GPIOB->CRL |=0x30000000;
	//P8
	GPIOB->CRH &=0xFFFFFFF0;
	GPIOB->CRH |=0x00000003;
	//P9
	GPIOB->CRH &=0xFFFFFF0F;
	GPIOB->CRH |=0x00000030;
*/
	GPIOB->CRL &=0x00FFFFFF;
	GPIOB->CRL |=0x33000000;
	
	GPIOB->CRH &=0xFFFFFF00;
	GPIOB->CRH |=0x00000033;
	
//3.设置上拉电阻 置为高电平
/*
	GPIOB->ODR|=1<<6;
	GPIOB->ODR|=1<<7;
	GPIOB->ODR|=1<<8;
	GPIOB->ODR|=1<<9;
*/
	GPIOB->ODR|=0xF<<6;

//	PBout(6)=1;
//	PBout(7)=1;
//	PBout(8)=1;
//	PBout(9)=1;
    
}

