#ifndef _BEEP_H_
#define _BEEP_H_

/*ͷ�ļ�����*/
#include "stm32f10x.h"
#include "sys.h"

/*�궨��*/
//#define BEEP_ON() {GPIOA->ODR |= 1<<6;}
//#define BEEP_OFF() {GPIOA->ODR &= ~(1<<6);}

//#define BEEP(x) if(x){GPIOA->ODR |= 1<<6;}else{GPIOA->ODR &= ~(1<<6);}

#define BEEP PAout(6)//GPIOA_ODR��6��λ

/*��������*/
void Beep_Init(void );

#endif
