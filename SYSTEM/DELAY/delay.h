#ifndef _DELAY_H_
#define _DELAY_H_

/*头文件引用*/
#include "stm32f10x.h"
#include <stdio.h>

/*宏定义*/

/*外部声明*/

/*函数声明*/
void delay_us(u32 us);
void delay_ms(u16 ms);
void delay_s(u16 s);

#endif
