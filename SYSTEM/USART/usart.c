#include "usart.h"

char 	g_ucUsart1Buff[USART_RX_SIZE];
u8 		g_ucUsart1RXFLAG = 0;
u32 	g_ucUsart1Cnt = 0;
char 	g_ucUsart3Buff[USART_RX_SIZE];
u8 		g_ucUsart3RXFLAG = 0;
u32 	g_ucUsart3Cnt = 0;

#define USART1_DMA_EN (0)

/*
    [发送]配置步骤：
    1. 通过在USART_CR1寄存器上置位UE位来激活USART
    2. 编程USART_CR1的M位来定义字长。
    3. 在USART_CR2中编程停止位的位数。
    4. 如果采用多缓冲器通信，配置USART_CR3中的DMA使能位(DMAT)。按多缓冲器通信中的描述配置DMA寄存器。
    5. 利用USART_BRR寄存器选择要求的波特率。
    6. 设置USART_CR1中的TE位，发送一个空闲帧作为第一次数据发送。
    7. 把要发送的数据写进USART_DR寄存器(此动作清除TXE位)。在只有一个缓冲器的情况下，对每个待发送的数据重复步骤7。
    8. 在USART_DR寄存器中写入最后一个数据字后，要等待TC=1，它表示最后一个数据帧的传输结束。
    当需要关闭USART或需要进入停机模式之前，需要确认传输结束，避免破坏最后一次传输。
*/
/*
****************************************************************************************************************
   
    函数原型:	void USARTx_initial(USART_TypeDef *USARTx,u32 _buad)
    函数功能: 初始化串口USARTx
    函数形参: _buad：待输入的波特率值
    硬件连接: 根据需求配置相应PA/B/C
    返回值:null
    波特率计算公式  brr = pclk / buad  注：buad为待设置的波特率值

****************************************************************************************************************  
*/
void USARTx_Init(USART_TypeDef *_USARTx,u32 _ucbuad)
{
    if(_USARTx == USART1)
    {
        /*GPIOA/B相关配置*/
        RCC->APB2ENR |= 1<<2;
        GPIOA->CRH &= 0xFFFFF00F;
        GPIOA->CRH |= 0x000008B0;//配置GPIOA(PA9)为复用推挽工作模式,配置GPIOA(PA10)为上拉输入模式
        /*USART相关配置*/
        RCC->APB2ENR |= 1<<14;
        RCC->APB2RSTR |=1<<14;
        RCC->APB2RSTR &=~(1<<14);
        _USARTx->BRR = (72*1000*1000) / _ucbuad;//形参填入波特率

        CM3_NVIC_SetPriority(USART1_IRQn,1,1);
    }
    else if(_USARTx == USART2)
    {
        
    }
    else if(_USARTx == USART3)
    {
        RCC->APB1ENR |= 1<<18;
        GPIOB->CRH &= 0xFFFF00FF;
        GPIOB->CRH |= 0x00008B00;//配置GPIOA(PA9)为复用推挽工作模式,配置GPIOA(PA10)为上拉输入模式
        /*USART相关配置*/
        RCC->APB1ENR |= 1<<18;
        RCC->APB1RSTR |=1<<18;
        RCC->APB1RSTR &=~(1<<18);
        _USARTx->BRR = (36*1000*1000) / _ucbuad;//形参填入波特率

        CM3_NVIC_SetPriority(USART3_IRQn,1,3);
    }
    _USARTx->CR1 |= 1<<13;   //串口激活-->USARTx使能
    _USARTx->CR1 |= 3<<2;   //接收、发送使能
    _USARTx->CR1 &=~(1<<12); //设置数据字长度：1个起始位 8个数据位 n个停止位
    _USARTx->CR2 &=~(3<<12); //设置1个停止位

#if USART1_DMA_EN
    USART1->CR3 |= 3<<6;//DMA-RX/TX Enable
    USART1->CR1 |= 1<<4;//总线空闲
#else
    /*RXNEIE：接收缓冲区非空中断使能 (RXNE interrupt enable)*/
    _USARTx->CR1 |= 1<<5;//当USART_SR中的ORE或者RXNE为1时，产生USART中断
#endif

}

/*
****************************************************************************************************************
    
    函数原型：_ARMABI int fputc(int c, FILE * stream) __attribute__((__nonnull__(2)));
    函数功能：printf重定向,实现向串口写数据
    函数形参：int c, FILE * stream
    硬件连接：无
    返回值：c
    
****************************************************************************************************************  
*/
int fputc(int c, FILE * stream)
{
	USART1->DR = c ;
	while(!(USART1->SR & 1<<7)){}//利用TXE标志位判断数据包是否发送完毕
    return c;
}

/*
****************************************************************************************************************
   
    函数原型: void USARTx_SendString(char *_pstr)
    函数功能: 通过串口x发送字符串
    函数形参: char *_pstr：待传入字符串的首地址
    硬件连接:
    返回值:null
    
****************************************************************************************************************  
*/
void USARTx_SendString(USART_TypeDef *_USARTx,char *_pstr)
{
    while(*_pstr!='\0')
    {
        _USARTx->DR = *_pstr++;
        while(!(_USARTx->SR & 1<<7));//利用TXE标志位判断数据包是否发送完毕
    }
}

#if USART1_DMA_EN
/*
****************************************************************************************************************
   
    函数原型: void USARTx_SendString_DMA(char *_pStr,u16 _usLen)
    函数功能: 通过串口1发送字符串
    函数形参: 
            *_pstr：待传入字符串的首地址
            _usLen: 存储器字符串长度
    硬件连接:
    返回值:null
    
****************************************************************************************************************  
*/
void USART1_SendString_DMA(char *_pStr,u16 _usLen)
{
    //USART1->CR3 |= 3<<6;//DMA--接收/发送使能
    DMAx_Channelx_Start(DMA1_Channel4,(u32)_pStr,(u32)&(USART1->DR),_usLen);
    while(!(DMA1->ISR & 1<<13));//在通道4产生了传输完成事件(TC)
    DMA1->ISR |= 1<<13;//清除传输完成标志位 
    while(!(USART1->SR & 1<<6));//等待发送完成(TC)
}
#endif

/*
****************************************************************************************************************
    
    函数原型：void USARTx_IRQHandler(void)
    函数功能：USARTx中断例程（即调用中断，读入数据）
    函数形参：无
    硬件连接：无
    返回值：无
    
****************************************************************************************************************  
*/
void USART1_IRQHandler(void)
{
#if USART1_DMA_EN
    if(USART1->SR & 1<<4)//当检测到总线空闲时，该位被硬件置位
    {
        //如果USART_CR1中的IDLEIE为’1’，则产生中断
        u8 ucTemp = USART1->DR ;//由软件序列清除该位(先读USART_SR，然后读USART_DR)
        g_ucUsart1Cnt = DMA1_Channel5->CNDTR;
        //g_ucUsart1Cnt = USART_RX_SIZE - DMA1_Channel5->CNDTR;
        g_ucUsartRXFLAG = 1;
        DMAx_Channelx_Start(DMA1_Channel5,(u32)g_ucUsartBuff,(u32)&(USART1->DR),USART_RX_SIZE);
    }
#else
	if(USART1->SR & 1<<5)//如果RXNE标志位为非空,则表示读数据寄存器非空,可往DR读入数据
	{
		u16 ucTemp = USART1->DR;//即读到一个字符,同时SR中的RXNE位清0
		if(g_ucUsart1Cnt < USART_RX_SIZE)//如果接收到数据且小于buff最大值
        {
            TIM3->CNT = 0;//计数从0开始
            TIM3->CR1 |= 1<<0;//开启计时器
            g_ucUsart1Buff[g_ucUsart1Cnt++]=ucTemp;//往缓冲区存入数据
        }
		/*else
			g_ucUsartRXFLAG = 1;//-->在定时器中断中*/
	}
#endif
}

/*
****************************************************************************************************************
    
    函数原型：void USARTx_IRQHandler(void)
    函数功能：USARTx中断例程（即调用中断，读入数据）
    函数形参：无
    硬件连接：无
    返回值：无
    
****************************************************************************************************************  
*/
void USART3_IRQHandler(void)
{
    if(USART3->SR & 1<<5)//如果RXNE标志位为非空,则表示读数据寄存器非空,可从DR读入数据
	{
		u16 ucTemp = USART3->DR;//即读到一个字符,同时SR中的RXNE位清0
		if(g_ucUsart3Cnt < USART_RX_SIZE)//如果接收到数据且小于buff最大值
        {
            TIM4->CNT = 0;//计数从0开始
            //if(g_ucUsart3Cnt == 0)
            TIM4->CR1 |= 1<<0;//开启计时器
            g_ucUsart3Buff[g_ucUsart3Cnt++]=ucTemp;//往缓冲区存入数据
        }
		/*else
			g_ucUsartRXFLAG = 1;//-->在定时器中断中*/
	}
}
