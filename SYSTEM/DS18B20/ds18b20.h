#ifndef _DS18B20_H_
#define _DS18B20_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include <stdio.h>

/*宏定义*/
#define DS18B20_DQ_OUT PBout(15)
#define DS18B20_DQ_IN PBin(15)
/*外部声明*/

/*函数声明*/
u8 DS18B20_Init(void);
float DS18B20_GetTemperature(void);

#endif
