#ifndef _DS18B20_H_
#define _DS18B20_H_

/*ͷ�ļ�����*/
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include <stdio.h>

/*�궨��*/
#define DS18B20_DQ_OUT PBout(15)
#define DS18B20_DQ_IN PBin(15)
/*�ⲿ����*/

/*��������*/
u8 DS18B20_Init(void);
float DS18B20_GetTemperature(void);

#endif
