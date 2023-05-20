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
#include <stdio.h>
#include <string.h>

char *WIFI_NAME="eldenass";
char *WIFI_PWD="sb123456";
u8 AP_Mode_FLAG,STA_Mode_FLAG;

//记得加\r\n
The_Server server_info_table[]={
    {"api.seniverse.com",80,\
    "GET https://api.seniverse.com/v3/weather/now.json?key=SfMynXOHCUppOPeLq&location=nanchang&language=en&unit=c\r\n"}
};

static void USART3_RxInfoParse(char *_pSrc, char *_pSub, char *_pRet);
int main(void)
{
	LED_Init();
	Beep_Init();
	DS18B20_Init();

	KEY_Init();
	OLED_Init();
	RTC_Init();
	RTC_SetStdTime(2000, 01, 01, 0, 0, 0, 0);

	USARTx_Init(USART1, 115200);
	USARTx_Init(USART3, 115200);
	TIMx_Init(TIM2, 7200, 1000 * 10 * 2); // 0.1ms * 1000*10*2 = 2s
	TIMx_Init(TIM3, 72, 1000 * 10);		  // 1us * 1000*10 = 10ms [USART1]
	TIMx_Init(TIM4, 72, 1000 * 10);		  // 1us * 1000*10 = 10ms [USART3]

	// 烧录成功提示
	BEEP = 1;
	delay_ms(1);
	BEEP = 0;

	while(1) //工作主频72MHz，执行一行代码消耗1/72us ~= 13.8ns
    {

        Select_Menu();
        
        if(g_ucUsart1RXFLAG)//通过串口1 接收校准时间
        {
            printf("USART1_RX:%s\n",g_ucUsart1Buff);
            RTC_Renew_Time(g_ucUsart1Buff);
            g_ucUsart1RXFLAG = 0;
            g_ucUsart1Cnt = 0;
            memset(g_ucUsart1Buff, 0, USART_RX_SIZE);
        }

        if(g_ucUsart3RXFLAG)
        {
            char text[50],code[10],temp[10];
        
            printf("USART3_RX:%s\n",g_ucUsart3Buff);
            IoT(g_ucUsart3Buff);//物联网控制 IoT

            if(STA_Mode_FLAG)
            {
                USART3_RxInfoParse(g_ucUsart3Buff,"text",text);
                USART3_RxInfoParse(g_ucUsart3Buff,"code",code);
                USART3_RxInfoParse(g_ucUsart3Buff,"temp",temp);
                printf("text=\"%s\",code=\"%s\",temperature=\"%s\"\n",text,code,temp);  
                STA_Mode_FLAG = 0;
            }
            g_ucUsart3RXFLAG = 0;
			g_ucUsart3Cnt = 0;
        }
	}
}
/*天气解析函数*/
static void USART3_RxInfoParse(char *_pSrc, char *_pSub, char *_pRet)
{
    char *p1=NULL,*p2=NULL;
    p1 = strstr(_pSrc, _pSub)+ strlen(_pSub) + 3;;
    if (p1 != NULL) //found sub srting!
    {
        p2 = strstr(p1, "\"");
        strncpy(_pRet, p1, p2-p1);
    }
}
