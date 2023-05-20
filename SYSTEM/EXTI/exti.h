#ifndef _EXTI_H_
#define _EXTI_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include <stdio.h>

/*宏定义*/

/*外部声明*/
extern u8 timer2_time_out;

/*函数声明*/
void EXTI_Init(void);
void EXTI0_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

#endif
