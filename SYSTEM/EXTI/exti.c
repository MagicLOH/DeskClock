#include "exti.h"

/*
****************************************************************************************************************
   
    函数原型: void EXTI_Initial(void)
    函数功能: 外部中断初始化
    函数形参: 无
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void EXTI_Init(void)
{
    /*GPIO相关配置*/
    KEY_Init();
    
    /*EXTI外部中断相关配置*///不需要时钟使能
    //开放请求
    EXTI->IMR |= 0x3<<0;//开放来自线PA0/PA1的中断请求
    EXTI->IMR |= 0x3<<4;//开放来自线PA4/PA5的中断请求
    //EXTI->EMR |= 0X3<<0;//开放来自线PA0/PA1的事件请求
    //EXTI->EMR |= 0X3<<4;//开放来自线PA4/PA5的事件请求
    EXTI->RTSR |=0X3<<0;//线0/1上的上升沿触发事件配置位
    EXTI->RTSR |=0X3<<4;//线4/5上的上升沿触发事件配置位
    EXTI->FTSR |=0X3<<0;//线0/1上的下降沿触发事件配置位
    EXTI->FTSR |=0X3<<4;//线0/1上的下降沿触发事件配置位
   
    /*通过AFIO_EXTICRx配置GPIO线上的外部中断/事件*/
    RCC->APB2ENR |= 1<<0;//使能AFIO时钟
    AFIO->EXTICR[0] &= ~(0xFF<<0);//选择EXTI0/1外部中断的输入源
    AFIO->EXTICR[1] &= ~(0xFF<<0);//选择EXTI4/5外部中断的输入源
    CM3_NVIC_SetPriority(EXTI0_IRQn,2,2);
    CM3_NVIC_SetPriority(EXTI1_IRQn,2,2);
    CM3_NVIC_SetPriority(EXTI4_IRQn,2,2);
    CM3_NVIC_SetPriority(EXTI9_5_IRQn,2,2);
}

/*
****************************************************************************************************************
   
    函数原型: void EXTIn_IRQHandler(void) (n=0/1/4/5)
    函数功能: 外部中断例程
    函数形参: 无
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void EXTI0_IRQHandler(void)
{
    if(EXTI->PR & 1<<0)
    {
        delay_ms(10);
        if (KEY0)   //上升沿触发
            USARTx_SendString(USART1,"上升沿触发\n");
        else
            USARTx_SendString(USART1,"下降沿触发\n");
        //置1清除标志位
        EXTI->PR |= 1<<1; 
    }
}
void EXTI1_IRQHandler(void)
{
    u32 AllCnts = 0;
    if(EXTI->PR & 1<<1)
    {
        delay_ms(10);
        if (!KEY1)   //下降沿触发
        {
            USARTx_SendString(USART1,"下降沿触发,计时开始\n");
            TIM2->CR1 = 0 ;
            TIM2->CR1 |= 1<<0;//开启定时器2
        }
        else
        {
            USARTx_SendString(USART1,"上升沿触发,计时结束\n");
            TIM2->CR1 &= ~(1<<0);//关闭定时器2
            AllCnts = ucOverFlowCnt * 0xFFFF + TIM2->CNT;
            printf("耗时 %d us %.2f s \n",AllCnts,AllCnts/1000.0/1000.0);
            ucOverFlowCnt = 0;
        }
        //置1清除标志位
        EXTI->PR |= 1<<1; 
    }
}
void EXTI4_IRQHandler(void)
{
    if(EXTI->PR & 1<<4)
    {
        delay_ms(10);
        if (!KEY2)   //下降沿触发
            USARTx_SendString(USART1,"下降沿触发\n");
        else
            USARTx_SendString(USART1,"上升沿触发\n");
        //置1清除标志位
        EXTI->PR |= 1<<1; 
    }
}
void EXTI9_5_IRQHandler(void)
{
    if(EXTI->PR & 1<<5)
    {
        delay_ms(10);
        if (!KEY3)   //下降沿触发
            USARTx_SendString(USART1,"下降沿触发\n");
        else
            USARTx_SendString(USART1,"上升沿触发\n");
        //置1清除标志位
        EXTI->PR |= 1<<1; 
    }
}

