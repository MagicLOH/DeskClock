#include "key.h"

/*
****************************************************************************************************************
    
    ����ԭ�ͣ�void KEY_Initial(void )
    �������ܣ���ʼ������ģʽ����������
    �����βΣ�null
    Ӳ�����ӣ�
            KEY0 -- PA0 -- �����ش��� -- UP 
            KEY1 -- PA1 -- �½��ش��� -- LEFT 
            KEY2 -- PA4 -- �½��ش��� -- DOWN 
            KEY3 -- PA5 -- �½��ش��� -- RIGHT
    �� �� ֵ��null
    
****************************************************************************************************************  
*/
void KEY_Init(void )
{
    //��ʼ��GPIOAʹ��ʱ��
    RCC->APB2ENR |= 1<<2;
    
    //PA0/1/4/5����Ϊ��/��������ģʽ
    GPIOA->CRL &= 0xFF00FF00;
    GPIOA->CRL |= 0x00880088;
    
    GPIOA->ODR &=~(1<<0);//�ֶ�����
    GPIOA->ODR |= 1<<1;//�ֶ�����
    GPIOA->ODR |= 1<<4;//�ֶ�����
    GPIOA->ODR |= 1<<5;//�ֶ�����
}

/*
****************************************************************************************************************
    
    ����ԭ�ͣ�unsigned char KEY_Scan(void ) 
    �������ܣ��������
    �����βΣ�null
    Ӳ�����ӣ�
    ����ֵ��null
    //typedef unsighed char uint8_t
    //typedef uint8_t u8

****************************************************************************************************************  
*/
u8 KEY_Scan(void)
{
    static u8 FLAG = 0;//0-->����û����  1-->�����Ѵ���
	//static ʹFLAG������������main������ͬ
    if(!FLAG && (KEY0 || !KEY1 || !KEY2 || !KEY3 ))
    {
        delay_ms(20);
        if(KEY0 || !KEY1 || !KEY2 || !KEY3 )
        {
        FLAG=1;
        if (KEY0)      return KEY0_PRES;
        else if(!KEY1) return KEY1_PRES;
        else if(!KEY2) return KEY2_PRES;
        else if(!KEY3) return KEY3_PRES;
        }
    }
    //������ɿ�����
    else if(FLAG && (!KEY0 && KEY1 && KEY2 && KEY3))
        FLAG=0;
		
    return 0;
}


