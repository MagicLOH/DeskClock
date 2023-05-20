#ifndef _TIMER_H_
#define _TIMER_H_
/*
    pclk    psc(分频系数)     cnt(HZ)     cnt+1(us/ms/s)      ARR_MAX      Longest_counting_Time
    72Mhz    1                   72M         1/72us            65535       1/72us * 65535 = 910us
    72Mhz    72                  1M          1us               65535       1us    * 65535 = 65ms      **frequency**
    72Mhz    7.2k                10k         0.1ms             65535       0.1ms  * 65535 = 6.5s      **frequency**
    72Mhz    36k                 2k          0.5ms             65535       0.5ms  * 65535 = 32.5s
*/
/*头文件引用*/
#include "stm32f10x.h"
#include "sys.h"
#include "usart.h"
#include "exti.h"
#include "ds18b20.h"
#include "menu.h"
#include <stdio.h>

/*宏定义*/

/*外部声明*/
extern u16 ucOverFlowCnt;
extern float RTC_Tmp;
extern u16 g_usOvertimeFLAG;

/*函数声明*/
void TIMx_Init(TIM_TypeDef * TIMx,u16 _usPSC,u16 _usARR);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);

#endif
