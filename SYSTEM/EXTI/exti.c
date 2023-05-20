#include "exti.h"

/*
****************************************************************************************************************
   
    ����ԭ��: void EXTI_Initial(void)
    ��������: �ⲿ�жϳ�ʼ��
    �����β�: ��
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void EXTI_Init(void)
{
    /*GPIO�������*/
    KEY_Init();
    
    /*EXTI�ⲿ�ж��������*///����Ҫʱ��ʹ��
    //��������
    EXTI->IMR |= 0x3<<0;//����������PA0/PA1���ж�����
    EXTI->IMR |= 0x3<<4;//����������PA4/PA5���ж�����
    //EXTI->EMR |= 0X3<<0;//����������PA0/PA1���¼�����
    //EXTI->EMR |= 0X3<<4;//����������PA4/PA5���¼�����
    EXTI->RTSR |=0X3<<0;//��0/1�ϵ������ش����¼�����λ
    EXTI->RTSR |=0X3<<4;//��4/5�ϵ������ش����¼�����λ
    EXTI->FTSR |=0X3<<0;//��0/1�ϵ��½��ش����¼�����λ
    EXTI->FTSR |=0X3<<4;//��0/1�ϵ��½��ش����¼�����λ
   
    /*ͨ��AFIO_EXTICRx����GPIO���ϵ��ⲿ�ж�/�¼�*/
    RCC->APB2ENR |= 1<<0;//ʹ��AFIOʱ��
    AFIO->EXTICR[0] &= ~(0xFF<<0);//ѡ��EXTI0/1�ⲿ�жϵ�����Դ
    AFIO->EXTICR[1] &= ~(0xFF<<0);//ѡ��EXTI4/5�ⲿ�жϵ�����Դ
    CM3_NVIC_SetPriority(EXTI0_IRQn,2,2);
    CM3_NVIC_SetPriority(EXTI1_IRQn,2,2);
    CM3_NVIC_SetPriority(EXTI4_IRQn,2,2);
    CM3_NVIC_SetPriority(EXTI9_5_IRQn,2,2);
}

/*
****************************************************************************************************************
   
    ����ԭ��: void EXTIn_IRQHandler(void) (n=0/1/4/5)
    ��������: �ⲿ�ж�����
    �����β�: ��
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void EXTI0_IRQHandler(void)
{
    if(EXTI->PR & 1<<0)
    {
        delay_ms(10);
        if (KEY0)   //�����ش���
            USARTx_SendString(USART1,"�����ش���\n");
        else
            USARTx_SendString(USART1,"�½��ش���\n");
        //��1�����־λ
        EXTI->PR |= 1<<1; 
    }
}
void EXTI1_IRQHandler(void)
{
    u32 AllCnts = 0;
    if(EXTI->PR & 1<<1)
    {
        delay_ms(10);
        if (!KEY1)   //�½��ش���
        {
            USARTx_SendString(USART1,"�½��ش���,��ʱ��ʼ\n");
            TIM2->CR1 = 0 ;
            TIM2->CR1 |= 1<<0;//������ʱ��2
        }
        else
        {
            USARTx_SendString(USART1,"�����ش���,��ʱ����\n");
            TIM2->CR1 &= ~(1<<0);//�رն�ʱ��2
            AllCnts = ucOverFlowCnt * 0xFFFF + TIM2->CNT;
            printf("��ʱ %d us %.2f s \n",AllCnts,AllCnts/1000.0/1000.0);
            ucOverFlowCnt = 0;
        }
        //��1�����־λ
        EXTI->PR |= 1<<1; 
    }
}
void EXTI4_IRQHandler(void)
{
    if(EXTI->PR & 1<<4)
    {
        delay_ms(10);
        if (!KEY2)   //�½��ش���
            USARTx_SendString(USART1,"�½��ش���\n");
        else
            USARTx_SendString(USART1,"�����ش���\n");
        //��1�����־λ
        EXTI->PR |= 1<<1; 
    }
}
void EXTI9_5_IRQHandler(void)
{
    if(EXTI->PR & 1<<5)
    {
        delay_ms(10);
        if (!KEY3)   //�½��ش���
            USARTx_SendString(USART1,"�½��ش���\n");
        else
            USARTx_SendString(USART1,"�����ش���\n");
        //��1�����־λ
        EXTI->PR |= 1<<1; 
    }
}

