#include "usart.h"

char 	g_ucUsart1Buff[USART_RX_SIZE];
u8 		g_ucUsart1RXFLAG = 0;
u32 	g_ucUsart1Cnt = 0;
char 	g_ucUsart3Buff[USART_RX_SIZE];
u8 		g_ucUsart3RXFLAG = 0;
u32 	g_ucUsart3Cnt = 0;

#define USART1_DMA_EN (0)

/*
    [����]���ò��裺
    1. ͨ����USART_CR1�Ĵ�������λUEλ������USART
    2. ���USART_CR1��Mλ�������ֳ���
    3. ��USART_CR2�б��ֹͣλ��λ����
    4. ������ö໺����ͨ�ţ�����USART_CR3�е�DMAʹ��λ(DMAT)�����໺����ͨ���е���������DMA�Ĵ�����
    5. ����USART_BRR�Ĵ���ѡ��Ҫ��Ĳ����ʡ�
    6. ����USART_CR1�е�TEλ������һ������֡��Ϊ��һ�����ݷ��͡�
    7. ��Ҫ���͵�����д��USART_DR�Ĵ���(�˶������TXEλ)����ֻ��һ��������������£���ÿ�������͵������ظ�����7��
    8. ��USART_DR�Ĵ�����д�����һ�������ֺ�Ҫ�ȴ�TC=1������ʾ���һ������֡�Ĵ��������
    ����Ҫ�ر�USART����Ҫ����ͣ��ģʽ֮ǰ����Ҫȷ�ϴ�������������ƻ����һ�δ��䡣
*/
/*
****************************************************************************************************************
   
    ����ԭ��:	void USARTx_initial(USART_TypeDef *USARTx,u32 _buad)
    ��������: ��ʼ������USARTx
    �����β�: _buad��������Ĳ�����ֵ
    Ӳ������: ��������������ӦPA/B/C
    ����ֵ:null
    �����ʼ��㹫ʽ  brr = pclk / buad  ע��buadΪ�����õĲ�����ֵ

****************************************************************************************************************  
*/
void USARTx_Init(USART_TypeDef *_USARTx,u32 _ucbuad)
{
    if(_USARTx == USART1)
    {
        /*GPIOA/B�������*/
        RCC->APB2ENR |= 1<<2;
        GPIOA->CRH &= 0xFFFFF00F;
        GPIOA->CRH |= 0x000008B0;//����GPIOA(PA9)Ϊ�������칤��ģʽ,����GPIOA(PA10)Ϊ��������ģʽ
        /*USART�������*/
        RCC->APB2ENR |= 1<<14;
        RCC->APB2RSTR |=1<<14;
        RCC->APB2RSTR &=~(1<<14);
        _USARTx->BRR = (72*1000*1000) / _ucbuad;//�β����벨����

        CM3_NVIC_SetPriority(USART1_IRQn,1,1);
    }
    else if(_USARTx == USART2)
    {
        
    }
    else if(_USARTx == USART3)
    {
        RCC->APB1ENR |= 1<<18;
        GPIOB->CRH &= 0xFFFF00FF;
        GPIOB->CRH |= 0x00008B00;//����GPIOA(PA9)Ϊ�������칤��ģʽ,����GPIOA(PA10)Ϊ��������ģʽ
        /*USART�������*/
        RCC->APB1ENR |= 1<<18;
        RCC->APB1RSTR |=1<<18;
        RCC->APB1RSTR &=~(1<<18);
        _USARTx->BRR = (36*1000*1000) / _ucbuad;//�β����벨����

        CM3_NVIC_SetPriority(USART3_IRQn,1,3);
    }
    _USARTx->CR1 |= 1<<13;   //���ڼ���-->USARTxʹ��
    _USARTx->CR1 |= 3<<2;   //���ա�����ʹ��
    _USARTx->CR1 &=~(1<<12); //���������ֳ��ȣ�1����ʼλ 8������λ n��ֹͣλ
    _USARTx->CR2 &=~(3<<12); //����1��ֹͣλ

#if USART1_DMA_EN
    USART1->CR3 |= 3<<6;//DMA-RX/TX Enable
    USART1->CR1 |= 1<<4;//���߿���
#else
    /*RXNEIE�����ջ������ǿ��ж�ʹ�� (RXNE interrupt enable)*/
    _USARTx->CR1 |= 1<<5;//��USART_SR�е�ORE����RXNEΪ1ʱ������USART�ж�
#endif

}

/*
****************************************************************************************************************
    
    ����ԭ�ͣ�_ARMABI int fputc(int c, FILE * stream) __attribute__((__nonnull__(2)));
    �������ܣ�printf�ض���,ʵ���򴮿�д����
    �����βΣ�int c, FILE * stream
    Ӳ�����ӣ���
    ����ֵ��c
    
****************************************************************************************************************  
*/
int fputc(int c, FILE * stream)
{
	USART1->DR = c ;
	while(!(USART1->SR & 1<<7)){}//����TXE��־λ�ж����ݰ��Ƿ������
    return c;
}

/*
****************************************************************************************************************
   
    ����ԭ��: void USARTx_SendString(char *_pstr)
    ��������: ͨ������x�����ַ���
    �����β�: char *_pstr���������ַ������׵�ַ
    Ӳ������:
    ����ֵ:null
    
****************************************************************************************************************  
*/
void USARTx_SendString(USART_TypeDef *_USARTx,char *_pstr)
{
    while(*_pstr!='\0')
    {
        _USARTx->DR = *_pstr++;
        while(!(_USARTx->SR & 1<<7));//����TXE��־λ�ж����ݰ��Ƿ������
    }
}

#if USART1_DMA_EN
/*
****************************************************************************************************************
   
    ����ԭ��: void USARTx_SendString_DMA(char *_pStr,u16 _usLen)
    ��������: ͨ������1�����ַ���
    �����β�: 
            *_pstr���������ַ������׵�ַ
            _usLen: �洢���ַ�������
    Ӳ������:
    ����ֵ:null
    
****************************************************************************************************************  
*/
void USART1_SendString_DMA(char *_pStr,u16 _usLen)
{
    //USART1->CR3 |= 3<<6;//DMA--����/����ʹ��
    DMAx_Channelx_Start(DMA1_Channel4,(u32)_pStr,(u32)&(USART1->DR),_usLen);
    while(!(DMA1->ISR & 1<<13));//��ͨ��4�����˴�������¼�(TC)
    DMA1->ISR |= 1<<13;//���������ɱ�־λ 
    while(!(USART1->SR & 1<<6));//�ȴ��������(TC)
}
#endif

/*
****************************************************************************************************************
    
    ����ԭ�ͣ�void USARTx_IRQHandler(void)
    �������ܣ�USARTx�ж����̣��������жϣ��������ݣ�
    �����βΣ���
    Ӳ�����ӣ���
    ����ֵ����
    
****************************************************************************************************************  
*/
void USART1_IRQHandler(void)
{
#if USART1_DMA_EN
    if(USART1->SR & 1<<4)//����⵽���߿���ʱ����λ��Ӳ����λ
    {
        //���USART_CR1�е�IDLEIEΪ��1����������ж�
        u8 ucTemp = USART1->DR ;//��������������λ(�ȶ�USART_SR��Ȼ���USART_DR)
        g_ucUsart1Cnt = DMA1_Channel5->CNDTR;
        //g_ucUsart1Cnt = USART_RX_SIZE - DMA1_Channel5->CNDTR;
        g_ucUsartRXFLAG = 1;
        DMAx_Channelx_Start(DMA1_Channel5,(u32)g_ucUsartBuff,(u32)&(USART1->DR),USART_RX_SIZE);
    }
#else
	if(USART1->SR & 1<<5)//���RXNE��־λΪ�ǿ�,���ʾ�����ݼĴ����ǿ�,����DR��������
	{
		u16 ucTemp = USART1->DR;//������һ���ַ�,ͬʱSR�е�RXNEλ��0
		if(g_ucUsart1Cnt < USART_RX_SIZE)//������յ�������С��buff���ֵ
        {
            TIM3->CNT = 0;//������0��ʼ
            TIM3->CR1 |= 1<<0;//������ʱ��
            g_ucUsart1Buff[g_ucUsart1Cnt++]=ucTemp;//����������������
        }
		/*else
			g_ucUsartRXFLAG = 1;//-->�ڶ�ʱ���ж���*/
	}
#endif
}

/*
****************************************************************************************************************
    
    ����ԭ�ͣ�void USARTx_IRQHandler(void)
    �������ܣ�USARTx�ж����̣��������жϣ��������ݣ�
    �����βΣ���
    Ӳ�����ӣ���
    ����ֵ����
    
****************************************************************************************************************  
*/
void USART3_IRQHandler(void)
{
    if(USART3->SR & 1<<5)//���RXNE��־λΪ�ǿ�,���ʾ�����ݼĴ����ǿ�,�ɴ�DR��������
	{
		u16 ucTemp = USART3->DR;//������һ���ַ�,ͬʱSR�е�RXNEλ��0
		if(g_ucUsart3Cnt < USART_RX_SIZE)//������յ�������С��buff���ֵ
        {
            TIM4->CNT = 0;//������0��ʼ
            //if(g_ucUsart3Cnt == 0)
            TIM4->CR1 |= 1<<0;//������ʱ��
            g_ucUsart3Buff[g_ucUsart3Cnt++]=ucTemp;//����������������
        }
		/*else
			g_ucUsartRXFLAG = 1;//-->�ڶ�ʱ���ж���*/
	}
}
