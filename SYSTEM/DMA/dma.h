#ifndef _DMA_H_
#define _DMA_H_

/*头文件引用*/
#include "stm32f10x.h"
#include <stdio.h>

/*宏定义*/
#define DMA_8Bit_Width  0
#define DMA_16Bit_Width 1
#define DMA_32Bit_Width 2

/*外部声明*/

/*函数声明*/
void DMA1_Channelx_MtoP_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth);
void DMA1_Channelx_PtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth);
void DMAx_Channelx_Start(DMA_Channel_TypeDef *_DMAx_Channelx,u32 _ulCmar,u32 _ulCpar,u16 _usCndtr);

#endif
