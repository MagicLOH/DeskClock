#ifndef _KEY_H_
#define _KEY_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "delay.h"
#include <stdio.h>

/*宏定义*/
//判断按键是否按下
#define KEY0 !!(GPIOA->IDR & 1<<0)
#define KEY1 !!(GPIOA->IDR & 1<<1)
#define KEY2 !!(GPIOA->IDR & 1<<4)
#define KEY3 !!(GPIOA->IDR & 1<<5)

#define KEY0_PRES 1 //按键按下返回值
#define KEY1_PRES 2
#define KEY2_PRES 3
#define KEY3_PRES 4

/*函数声明*/
u8 KEY_Scan(void );
void KEY_Init(void );

#endif

