#ifndef _ESP8266_H_
#define _ESP8266_H_

/*ͷ�ļ�����*/
#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

/*�궨��*/
#define TCP_Server_Name "TCP666"
#define TCP_Server_Password "12345678"

/*�ⲿ����*/

/*��������*/
u8 ESP8266_TCPServer_Init(char *_pSSID,char *_pPwd);
u8 ESP8266_TCPClient_Init(char *_pSSID,char *_pPwd);
u8 ESP8266_SendData2Client(u16 _usLux,float _fTemp);
u8 ESP8266_SendData2Server(char *_usDat,char *_pRemoteIP, u16 _usRemotePort);

#endif
