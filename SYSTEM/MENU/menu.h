#ifndef _MENU_H_
#define _MENU_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "key.h"
#include "oled.h"
#include "rtc.h"
#include "timer.h"
#include "while1.h"
#include <stdio.h>

typedef struct
{
unsigned char CurrentTaskNum; //当前任务编号:页码
unsigned char Up;             //上键
unsigned char Down;           //下键
unsigned char Left;           //返回
unsigned char Right;          //确认键
void (*CurrentTaskFunc)(void);//当前操作（函数指针）
}taskInfo;


/*宏定义*/

/*外部声明*/
extern u8 taskIndex;
extern void (*taskFunc)(void);
extern taskInfo taskTable[];

/*函数声明*/
void Select_Menu(void);

#endif





