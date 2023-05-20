#ifndef _BEEP_H_
#define _BEEP_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "sys.h"

/*宏定义*/
//#define BEEP_ON() {GPIOA->ODR |= 1<<6;}
//#define BEEP_OFF() {GPIOA->ODR &= ~(1<<6);}

//#define BEEP(x) if(x){GPIOA->ODR |= 1<<6;}else{GPIOA->ODR &= ~(1<<6);}

#define BEEP PAout(6)//GPIOA_ODR的6号位

/*函数声明*/
void Beep_Init(void );

#endif
