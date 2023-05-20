#ifndef _DMA_H_
#define _DMA_H_

/*ͷ�ļ�����*/
#include "stm32f10x.h"
#include <stdio.h>

/*�궨��*/
#define DMA_8Bit_Width  0
#define DMA_16Bit_Width 1
#define DMA_32Bit_Width 2

/*�ⲿ����*/

/*��������*/
void DMA1_Channelx_MtoP_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth);
void DMA1_Channelx_PtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth);
void DMAx_Channelx_Start(DMA_Channel_TypeDef *_DMAx_Channelx,u32 _ulCmar,u32 _ulCpar,u16 _usCndtr);

#endif
