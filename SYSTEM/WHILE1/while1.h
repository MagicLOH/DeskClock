#ifndef _WHILE1_H_
#define _WHILE1_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "exti.h"
#include "rtc.h"
#include "ds18b20.h"
#include "oled.h"
#include "dma.h"
#include "esp8266.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>

typedef struct{
    char ip[20];
    u16 port;
    char url[500];
}The_Server;

/*宏定义*/


/*外部声明*/
extern char *WIFI_NAME;
extern char *WIFI_PWD;
extern u8 AP_Mode_FLAG,STA_Mode_FLAG;
extern The_Server server_info_table[1];

/*函数声明*/
void Cycling_Program(void);



#endif
