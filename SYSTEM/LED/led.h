#ifndef _LED_H_
#define _LED_H_

/*头文件引用*/
#include "stm32f10x.h"

/*宏定义*/

//#define LED1_ON()     {GPIOB->ODR &= ~(1<<6);}
//#define LED1_OFF()    {GPIOB->ODR |= (1<<6);}

//#define LED1(x) if(x){GPIOB->ODR |= 1<<6;}else{GPIOB->ODR &= ~(1<<6);}
//#define LED2(x) if(x){GPIOB->ODR |= 1<<7;}else{GPIOB->ODR &= ~(1<<7);}
//#define LED3(x) if(x){GPIOB->ODR |= 1<<8;}else{GPIOB->ODR &= ~(1<<8);}
//#define LED4(x) if(x){GPIOB->ODR |= 1<<9;}else{GPIOB->ODR &= ~(1<<9);}

#define LED1 PBout(6)//位带操作-->ODR第六位
#define LED2 PBout(7)
#define LED3 PBout(8)
#define LED4 PBout(9)

#define LED_ALL_ON()  {GPIOB->ODR &= ~(0xF<<6);}
#define LED_ALL_OFF()  {GPIOB->ODR |= (0xF<<6);}

/*函数声明*/
void LED_Init(void );

#endif

