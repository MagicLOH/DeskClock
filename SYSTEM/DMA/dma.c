#include "dma.h"

/*
****************************************************************************************************************
   
    函数原型: void DMA1_Channelx_MtoP_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
    函数功能: DMA1 通道初始化[存储器到片上外设]
    函数形参: 
            _DMA1_Channelx -->DMA通道
            _ucWidth -->数据宽度
    硬件连接: 无
    返 回 值: 无
    
    通道配置过程
    下面是配置DMA通道x的过程(x代表通道号)：
    1. 在DMA_CPARx寄存器中设置外设寄存器的地址。发生外设数据传输请求时，这个地址将
    是数据传输的源或目标。
    2. 在DMA_CMARx寄存器中设置数据存储器的地址。发生外设数据传输请求时，传输的数
    据将从这个地址读出或写入这个地址。
    3. 在DMA_CNDTRx寄存器中设置要传输的数据量。在每个数据传输后，这个数值递减。
    4. 在DMA_CCRx寄存器的PL[1:0]位中设置通道的优先级。
    5. 在DMA_CCRx寄存器中设置数据传输的方向、循环模式、外设和存储器的增量模式、外
    设和存储器的数据宽度、传输一半产生中断或传输完成产生中断。
    6. 设置DMA_CCRx寄存器的ENABLE位，启动该通道。
****************************************************************************************************************  
*/
void DMA1_Channelx_MtoP_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
{
    /*外设时钟使能*/
    RCC->AHBENR |= 1<<0;

    /*配置 DMA_Channelx 功能*/
    _DMA1_Channelx->CCR = 0x00000000; //手动复位全部清零
    _DMA1_Channelx->CCR |= 0<<0; //EN:通道开启-1/关闭-0(channel enable)
    _DMA1_Channelx->CCR |= 1<<1; //TCIE:允许传输完成中断 (Transfer complete interrupt enable)
    _DMA1_Channelx->CCR |= 1<<4; //DIR:数据传输方向[从存储器读-1/从外设读-0] (Data transfer direction)
    _DMA1_Channelx->CCR |= 0<<5; //循环模式 off-0/on-1
    _DMA1_Channelx->CCR |= 0<<6; //PINC:外设地址增量模式[开1/关0] (Peripheral increment mode)
    _DMA1_Channelx->CCR |= 1<<7; //MINC:存储器地址增量模式[开1/关0] (Memory increment mode)
    _DMA1_Channelx->CCR |= _ucWidth<<8; //PSIZE[8:9]：外设数据宽度 (Peripheral size)
    _DMA1_Channelx->CCR |= _ucWidth<<10;//MSIZE[10:11]：存储器数据宽度 (Memory size)
    _DMA1_Channelx->CCR |= 3<<12;//PL[12:13]：通道优先级 (Channel priority level)
    _DMA1_Channelx->CCR |= 0<<14;//非存储器到存储器模式-0/存储器到存储器模式-1
}

/*
****************************************************************************************************************
   
    函数原型: void DMA1_Channelx_PtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
    函数功能: DMA1 通道初始化[片上外设到存储器]
    函数形参: 
            _DMA1_Channelx -->DMA通道x
            _ucWidth -->数据宽度
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void DMA1_Channelx_PtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
{
    /*外设时钟使能*/
    RCC->AHBENR |= 1<<0;

    /*配置 DMA_Channelx 功能*/
    _DMA1_Channelx->CCR = 0x00000000; //手动复位全部清零
    _DMA1_Channelx->CCR |= 0<<0; //EN:通道开启-1/关闭-0(channel enable)
    _DMA1_Channelx->CCR |= 0<<1; //TCIE:允许传输完成中断 (Transfer complete interrupt enable)
    _DMA1_Channelx->CCR |= 0<<4; //DIR:数据传输方向[从存储器读-1/从外设读-0] (Data transfer direction)
    _DMA1_Channelx->CCR |= 0<<5; //循环模式 off-0/on-1
    _DMA1_Channelx->CCR |= 0<<6; //PINC:外设地址增量模式[开1/关0] (Peripheral increment mode)
    _DMA1_Channelx->CCR |= 1<<7; //MINC:存储器地址增量模式[开1/关0] (Memory increment mode)
    _DMA1_Channelx->CCR |= _ucWidth<<8; //PSIZE[8:9]：外设数据宽度 (Peripheral size)
    _DMA1_Channelx->CCR |= _ucWidth<<10;//MSIZE[10:11]：存储器数据宽度 (Memory size)
    _DMA1_Channelx->CCR |= 3<<12;//PL[12:13]：通道优先级 (Channel priority level)
    _DMA1_Channelx->CCR |= 0<<14;//非存储器到存储器模式-0/存储器到存储器模式-1
}

/*
****************************************************************************************************************
   
    函数原型: void DMA1_Channelx_MtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
    函数功能: DMA1 通道初始化[存储器到存储器]
    函数形参: _DMA1_Channelx -->DMA通道x, _ucWidth -->数据宽度
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void DMA1_Channelx_MtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
{
    /*外设时钟使能*/
    RCC->AHBENR |= 1<<0;

    /*配置 DMA_Channelx 功能*/
    _DMA1_Channelx->CCR = 0x00000000; //手动复位全部清零
    _DMA1_Channelx->CCR |= 0<<0; //EN:通道开启-1/关闭-0(channel enable)
    _DMA1_Channelx->CCR |= 0<<1; //TCIE:允许传输完成中断 (Transfer complete interrupt enable)
    _DMA1_Channelx->CCR |= 0<<4; //DIR:数据传输方向[从存储器读-1/从外设读-0] (Data transfer direction)
    _DMA1_Channelx->CCR |= 0<<5; //循环模式 off-0/on-1
    _DMA1_Channelx->CCR |= 0<<6; //PINC:外设地址增量模式[开1/关0] (Peripheral increment mode)
    _DMA1_Channelx->CCR |= 1<<7; //MINC:存储器地址增量模式[开1/关0] (Memory increment mode)
    _DMA1_Channelx->CCR |= _ucWidth<<8; //PSIZE[8:9]：外设数据宽度 (Peripheral size)
    _DMA1_Channelx->CCR |= _ucWidth<<10;//MSIZE[10:11]：存储器数据宽度 (Memory size)
    _DMA1_Channelx->CCR |= 3<<12;//PL[12:13]：通道优先级 (Channel priority level)
    _DMA1_Channelx->CCR |= 1<<14;//非存储器到存储器模式-0/存储器到存储器模式-1
}

/*
****************************************************************************************************************
   
    函数原型: void DMA_Channelx_Start(DMA_Channel_TypeDef *_DMA1_Channelx,u32 _ulCmar,u32 _ulCpar,u16 _usCndtr)
    函数功能: 开启 DMA 通道传输
    函数形参: _DMA1_Channelx 通道x
            _ulCmar 存储器地址
            _ulCpar 外设地址
            _usCndtr 传输数据量
    硬件连接: 无
    返 回 值: 无

****************************************************************************************************************  
*/
void DMAx_Channelx_Start(DMA_Channel_TypeDef *_DMAx_Channelx,u32 _ulCmar,u32 _ulCpar,u16 _usCndtr)
{
    _DMAx_Channelx->CCR &= ~(1<<0);  //Open the channel
    _DMAx_Channelx->CMAR = _ulCmar;  //存储器地址
    _DMAx_Channelx->CNDTR = _usCndtr;//串数的数据量(依次递减)
    _DMAx_Channelx->CPAR = _ulCpar; //外设地址
    _DMAx_Channelx->CCR |= 1<<0;     //Close the channel

}
