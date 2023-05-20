#include "led.h"

/*
**************************************************************************************************
    
		�������ܣ�LED��ʼ��
    Ӳ�����ӣ�
            LED 1 -- PB6 --�͵�ƽ����
            LED 2 -- PB7 --�͵�ƽ����
            LED 3 -- PB8 --�͵�ƽ����
            LED 4 -- PB9 --�͵�ƽ����
						
**************************************************************************************************
*/
void LED_Init(void )
{
    //1.����ʱ��
	
    RCC->APB2ENR |=1<<3;
	
    //2.����P6/7/8/9Ϊͨ���������ģʽ Ĭ�ϵ͵�ƽ
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
	
//3.������������ ��Ϊ�ߵ�ƽ
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

