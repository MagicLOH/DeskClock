#ifndef _USART_H_
#define _USART_H_

/*头文件引用*/ 
#include "stm32f10x.h"
#include <stdio.h>
#include "key.h"
#include "delay.h"
#include "sys.h"
#include "dma.h"

/*宏定义*/
#define USART_RX_SIZE 1024

/*外部声明*/
extern char g_ucUsart1Buff[USART_RX_SIZE];
extern u8   g_ucUsart1RXFLAG;
extern u32  g_ucUsart1Cnt;
extern char g_ucUsart3Buff[USART_RX_SIZE];
extern u8 	g_ucUsart3RXFLAG;
extern u32 	g_ucUsart3Cnt;

/*函数声明*/
int fputc(int c, FILE * stream);
void USARTx_Init(USART_TypeDef *_USARTx,u32 _ucbuad);
void USARTx_SendString(USART_TypeDef *,char *_pstr);
void USART1_SendString_DMA(char *_pStr,u16 _usLen);
void USART1_IRQHandler(void);

#endif

