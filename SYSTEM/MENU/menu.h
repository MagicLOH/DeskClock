#ifndef _MENU_H_
#define _MENU_H_

/*ͷ�ļ�����*/
#include "stm32f10x.h"
#include "key.h"
#include "oled.h"
#include "rtc.h"
#include "timer.h"
#include "while1.h"
#include <stdio.h>

typedef struct
{
unsigned char CurrentTaskNum; //��ǰ������:ҳ��
unsigned char Up;             //�ϼ�
unsigned char Down;           //�¼�
unsigned char Left;           //����
unsigned char Right;          //ȷ�ϼ�
void (*CurrentTaskFunc)(void);//��ǰ����������ָ�룩
}taskInfo;


/*�궨��*/

/*�ⲿ����*/
extern u8 taskIndex;
extern void (*taskFunc)(void);
extern taskInfo taskTable[];

/*��������*/
void Select_Menu(void);

#endif





