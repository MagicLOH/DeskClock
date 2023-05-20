#include "key.h"

/*
****************************************************************************************************************
    
    函数原型：void KEY_Initial(void )
    函数功能：初始化按键模式，下拉输入
    函数形参：null
    硬件连接：
            KEY0 -- PA0 -- 上升沿触发 -- UP 
            KEY1 -- PA1 -- 下降沿触发 -- LEFT 
            KEY2 -- PA4 -- 下降沿触发 -- DOWN 
            KEY3 -- PA5 -- 下降沿触发 -- RIGHT
    返 回 值：null
    
****************************************************************************************************************  
*/
void KEY_Init(void )
{
    //初始化GPIOA使能时钟
    RCC->APB2ENR |= 1<<2;
    
    //PA0/1/4/5设置为上/下拉输入模式
    GPIOA->CRL &= 0xFF00FF00;
    GPIOA->CRL |= 0x00880088;
    
    GPIOA->ODR &=~(1<<0);//手动下拉
    GPIOA->ODR |= 1<<1;//手动上拉
    GPIOA->ODR |= 1<<4;//手动上拉
    GPIOA->ODR |= 1<<5;//手动上拉
}

/*
****************************************************************************************************************
    
    函数原型：unsigned char KEY_Scan(void ) 
    函数功能：按键检测
    函数形参：null
    硬件连接：
    返回值：null
    //typedef unsighed char uint8_t
    //typedef uint8_t u8

****************************************************************************************************************  
*/
u8 KEY_Scan(void)
{
    static u8 FLAG = 0;//0-->按键没处理  1-->按键已处理
	//static 使FLAG的生命周期与main函数相同
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
    //处理后松开按键
    else if(FLAG && (!KEY0 && KEY1 && KEY2 && KEY3))
        FLAG=0;
		
    return 0;
}


