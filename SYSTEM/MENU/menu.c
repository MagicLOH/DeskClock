#include "menu.h"

taskInfo taskTable[];

void fun0(void)
{
    OLED_DrawSrting(0,16*0,8,16,"    Main Menu   ",0);
    OLED_DrawBMP(0,16*1,32,32,Clock_32x32Bmp_ColLn,M_ColLn,0);
    OLED_DrawBMP(48-1,16*1,32,32,Temp_32x32Bmp_ColLn,M_ColLn,0);
    OLED_DrawBMP(80+16,16*1,32,32,WIFI_32x32Bmp_ColLn,M_ColLn,0);
    OLED_DrawSrting(0,16*3,8,16," Press any key..",0);
}
void fun1(void)
{
    OLED_DrawSrting(0,16*0,8,16,"  Select Menu  ",0);
    OLED_DrawSrting(0,16*1,8,16,"[fun1] RTC",1);
    OLED_DrawSrting(0,16*2,8,16,"[fun2] DS18B20",0);
    OLED_DrawSrting(0,16*3,8,16,"[fun3] WIFI",0);

}
void fun2(void)
{
    OLED_DrawSrting(0,16*0,8,16,"  Select Menu  ",0);
    OLED_DrawSrting(0,16*1,8,16,"[fun1] RTC",0);
    OLED_DrawSrting(0,16*2,8,16,"[fun2] DS18B20",1);
    OLED_DrawSrting(0,16*3,8,16,"[fun3] WIFI",0);
    TIM2->CR1 &= ~(1<<0);//TIM2-OFF
}
void fun3(void)
{
    OLED_DrawSrting(0,16*0,8,16,"  Select Menu  ",0);
    OLED_DrawSrting(0,16*1,8,16,"[fun1] RTC",0);
    OLED_DrawSrting(0,16*2,8,16,"[fun2] DS18B20",0);
    OLED_DrawSrting(0,16*3,8,16,"[fun3] WIFI",1);

}
void fun1_sub1(void)
{
    char date[100];
    char time[100];
    char weekday[5];
    OLED_DrawSrting(0,16*0,8,16,"***<RTC__CLK>***",0);
    sprintf(date,"%04d/%02d/%02d",Clock_Calendar.usYear,Clock_Calendar.ucMonth,Clock_Calendar.ucDay);
    sprintf(time,"%02d:%02d:%02d",Clock_Calendar.ucHour,Clock_Calendar.ucMin,Clock_Calendar.ucSec);
    sprintf(weekday,"[%d]",Clock_Calendar.ucWeek);
    OLED_DrawSrting(0,16,8,16,">>Beijing time<<",0);
    OLED_DrawSrting(0,16*2,8,16,date,1);
    OLED_DrawSrting(0,16*3,8,16,time,1);
    OLED_DrawChinese(72,16*3,16,0,1);//星
    OLED_DrawChinese(72+16,16*3,16,1,1);//期
    OLED_DrawSrting(72+16*2,16*3,8,16,weekday,1);
}
void fun2_sub1(void)
{
    char tmp[20];
    float ftmp;
    OLED_DrawSrting(0,16*0,8,16,"*****<Temp>*****",0);
    OLED_DrawSrting(0,16*1,8,16,"Current",0);
    OLED_DrawSrting(0,16*2,8,16,"temperature:",0);
    ftmp = DS18B20_GetTemperature();
    sprintf(tmp,">>>>>>%.1f<<<<<<",ftmp);
    OLED_DrawSrting(0,16*3,8,16,tmp,1);

}
void fun3_sub1(void)
{
    OLED_DrawSrting(0,16*0,8,16,"   WIFI Menu   ",0);
    OLED_DrawSrting(0,16*1,8,16,"[1] SoftAP_Mode",1);
    OLED_DrawSrting(0,16*2,8,16,"[2] STA_Mode",0);
    
}
void fun3_sub2(void)
{
    OLED_DrawSrting(0,16*0,8,16,"   WIFI Menu   ",0);
    OLED_DrawSrting(0,16*1,8,16,"[1] SoftAP_Mode",0);
    OLED_DrawSrting(0,16*2,8,16,"[2] STA_Mode",1);
}
void f3_s1_func(void)
{
    OLED_DrawSrting(0,16*0,8,16,"Press [Up] to start SoftAP Mode",1);
    if(KEY0)
    {
        OLED_DrawSrting(0,16*2,8,16," SoftAP Mode ON!",1);
        u8 ret = ESP8266_TCPServer_Init(TCP_Server_Name,TCP_Server_Password);
        if(ret != 0) printf("Error num >>%d<< !\n",ret);
    }
    if(!KEY2)
        {
            OLED_DrawSrting(0,16*2,8,16,"Already send Data!",1);
            u8 retr = ESP8266_SendData2Client(23333,23.33);
            if(retr == 0) printf("ret = %d,Send success...\n",retr);
            else          printf("ret = %d,Send fail!!!\n",retr);
        }
    OLED_DrawSrting(0,16*3,8,16,"[Left] to return",0);
}
void f3_s2_func(void)
{
    OLED_DrawSrting(0,16*0,8,16,"Press [Up] to start STA Mode",1);
    if(KEY0)//button [up]
        {
            OLED_DrawSrting(0,16*2,8,16,"Station Mode ON!",1);
            u8 ret = ESP8266_TCPClient_Init(WIFI_NAME,WIFI_PWD);
            if(ret != 0) printf("Error num >>%d<< !\n",ret);
        }
    if(!KEY2)//button [down] 
        {
            u8 ret = ESP8266_SendData2Server(server_info_table[0].url,server_info_table[0].ip,\
                                             server_info_table[0].port);
            if(ret == 0) printf("ret = %d,Send success...\n",ret);
            else         printf("ret = %d,Send fail!!!\n",ret);
            STA_Mode_FLAG = 1;
        }
        
    OLED_DrawSrting(0,16*3,8,16,"[Left] to return",0);
}

taskInfo taskTable[] =
{
    {0, 1, 1, 1, 1, fun0},
    {1, 3, 2, 0, 4, fun1},
    {2, 1, 3, 0, 5, fun2},
    {3, 2, 1, 0, 6, fun3},
    {4, 4, 4, 1, 4, fun1_sub1},
    {5, 5, 5, 2, 5, fun2_sub1},
    {6, 7, 7, 3, 8, fun3_sub1},
    {7, 6, 6, 3, 9, fun3_sub2},
    {8, 8, 8, 6, 8, f3_s1_func},
    {9, 9, 9, 7, 9, f3_s2_func},
    
};

u8 taskIndex;
void (*taskFunc)(void);

void Select_Menu(void)
{
    switch(KEY_Scan())
        {
            case KEY0_PRES: taskIndex = taskTable[taskIndex].Up;    break; //前进
            case KEY1_PRES: taskIndex = taskTable[taskIndex].Left;  break; //后退
            case KEY2_PRES: taskIndex = taskTable[taskIndex].Down;  break; //返回
            case KEY3_PRES: taskIndex = taskTable[taskIndex].Right; break; //确认
        }
        //更新任务
        taskFunc = taskTable[taskIndex].CurrentTaskFunc;
        OLED_GRAM_Reset(); //初始化 GRAM 显存
        (*taskFunc)();
        OLED_GRAM_Refresh();//刷新 GRAM
}

