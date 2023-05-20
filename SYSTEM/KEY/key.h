#ifndef _KEY_H_
#define _KEY_H_

/*ͷ�ļ�����*/
#include "stm32f10x.h"
#include "delay.h"
#include <stdio.h>

/*�궨��*/
//�жϰ����Ƿ���
#define KEY0 !!(GPIOA->IDR & 1<<0)
#define KEY1 !!(GPIOA->IDR & 1<<1)
#define KEY2 !!(GPIOA->IDR & 1<<4)
#define KEY3 !!(GPIOA->IDR & 1<<5)

#define KEY0_PRES 1 //�������·���ֵ
#define KEY1_PRES 2
#define KEY2_PRES 3
#define KEY3_PRES 4

/*��������*/
u8 KEY_Scan(void );
void KEY_Init(void );

#endif

