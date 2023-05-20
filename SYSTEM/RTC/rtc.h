#ifndef _RTC_H_
#define _RTC_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "ds18b20.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*结构体定义*/
struct rtc_clock
{   
    u16 usYear;
    u8 ucMonth;
    u8 ucDay;
    u8 ucHour;
    u8 ucMin;
    u8 ucSec;
    u8 ucWeek;
};

/*宏定义*/
#define leap_year 1
#define common_year 0

/*外部声明*/
extern struct rtc_clock Clock_Calendar;//最后的标准时间格式输出结构体变量

/*函数声明*/
void RTC_Init(void );
void RTC_IRQHandler(void);
void RTC_SecToStd(u32 _ulTotalSec);
void RTC_SetStdTime(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek);
void RTC_Renew_Time(char *_p);

#endif
