#ifndef _ESP8266_H_
#define _ESP8266_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

/*宏定义*/
#define TCP_Server_Name "TCP666"
#define TCP_Server_Password "12345678"

/*外部声明*/

/*函数声明*/
u8 ESP8266_TCPServer_Init(char *_pSSID,char *_pPwd);
u8 ESP8266_TCPClient_Init(char *_pSSID,char *_pPwd);
u8 ESP8266_SendData2Client(u16 _usLux,float _fTemp);
u8 ESP8266_SendData2Server(char *_usDat,char *_pRemoteIP, u16 _usRemotePort);

#endif
