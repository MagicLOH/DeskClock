#include "esp8266.h"

/*
****************************************************************************************************************
   
    函数原型: static u8 ESP8266_SendATCmd(char *_pCmd,char *_pAck)
    函数功能: 通过串口3 向ESP8266 发送AT功能指令
    函数形参: 
            _pCmd AT指令
            _pAck 回复的应答(OK等)
    硬件连接: 无
    返 回 值: 成功-0/失败-1
    
****************************************************************************************************************  
*/
static u8 ESP8266_SendATCmd(char *_pCmd,char *_pAck,u16 _usOvertime)
{
#if 0
    u16 count = 0;
    for(u8 i=0;i<5;i++)//最多发送五遍
    {
        g_ucUsart3Cnt = 0;
        g_ucUsart3RXFLAG = 0;
        USARTx_SendString(USART3,_pCmd);

        for(u8 j=0;j<3;j++)//最多三段数据
        {
            while(g_ucUsart3RXFLAG == 0)
            {
                delay_ms(1);
                if(count++ % 50 == 0) LED2 = !LED2;
                if(_usOvertime == count) break;//g_ucUsart3RXFLAG = 1;
            }
            if(g_ucUsart3RXFLAG == 0) continue;//如果RX标志位仍为0 进入下一轮
            //g_ucUsart3RXFLAG = 0;
            if(strstr(g_ucUsart3Buff,_pAck) == NULL) ;//12 346""
            else 
            {
                LED2 = 1;//灯灭
                g_ucUsart3Cnt = 0;
                g_ucUsart3RXFLAG = 0;
                return 0;//成功
            }
        }
    }
    //fail, program exit
    LED2 = 1;
    g_ucUsart3RXFLAG = 0;
    g_ucUsart3Cnt = 0;
    return 1;
#endif
    u16 count = 0;
    for(u8 i=0;i<5;i++)//最多发送五遍
    {
        g_ucUsart3Cnt = 0;
        g_ucUsart3RXFLAG = 0;
        USARTx_SendString(USART3,_pCmd);

        for(u8 j=0;j<3;j++)//最多三段数据
        {
            count = 0;
            while(g_ucUsart3RXFLAG == 0)
            {
                delay_ms(1);
                count++;
                if(count % 50 == 0) LED2 = !LED2;
                if(_usOvertime == count) break;//g_ucUsart3RXFLAG = 1;
            }
            if(g_ucUsart3RXFLAG == 0) continue;//如果RX标志位仍为0 进入下一轮
            
            g_ucUsart3RXFLAG = 0;
            if(strstr(g_ucUsart3Buff,_pAck)) //12 346""
            {
                LED2 = 1;//灯灭
                g_ucUsart3Cnt = 0;
                //g_ucUsart3RXFLAG = 0;
                return 0;//成功
            }
        }
    }
    //fail, program exit
    LED2 = 1;
    g_ucUsart3RXFLAG = 0;
    g_ucUsart3Cnt = 0;
    return 1;
}

/*
****************************************************************************************************************
   
    函数原型: u8 ESP8266_TCPServer_Init(char *_pSSID,char *_pPwd)
    函数功能: SoftAP模式功能配置
    函数形参: 
            _pSSID WIFI名称
            _pPwd  WIFI密码
    硬件连接: 无
    返 回 值: 0成功/1失败
    
****************************************************************************************************************  
*/
u8 ESP8266_TCPServer_Init(char *_pSSID,char *_pPwd)
{
    char buf[50],ip[20],mac[20];
    char *pIP = NULL;
    char *pMAC = NULL;
    char *pEnd = NULL;
    
    printf("0.设备重启..........");
    USARTx_SendString(USART3, "AT+RST\r\n");
    delay_ms(1000);
    delay_ms(1000);
    printf("ok.\n");
    
    printf("1.配置为AP模式......");
    if(ESP8266_SendATCmd("AT+CWMODE=2\r\n","OK",100)) return 1;
    printf("ok.\n");

    printf("2.配置AP信息........");
    sprintf(buf,"AT+CWSAP_DEF=\"%s\",\"%s\",5,4\r\n",_pSSID,_pPwd);
    //if(ESP8266_SendATCmd(buf,"OK")) return 2;
    printf("ok.\n"); 

    printf("3.查询本机IP地址....");
    if(ESP8266_SendATCmd("AT+CIFSR\r\n","OK",100)) return 3;
    printf("ok.\n");
    //+CIFSR:APIP,"192.168.4.1" 
    //+CIFSR:APMAC,"a2:20:a6:19:c7:0a"
    pIP = strstr(g_ucUsart3Buff,"APIP") + 6;
    pMAC = strstr(g_ucUsart3Buff,"APMAC") +7;
    if((pIP != NULL) && (pMAC != NULL))
    {
        pEnd = strstr(pIP,"\"");//find "\"'s first appeared address
        strncpy(ip,pIP,pEnd-pIP);
        pEnd = NULL;
        pEnd = strstr(pMAC,"\"");
        strncpy(mac,pMAC,pEnd-pMAC);
    }

    printf("4.开启多链接........");
    if(ESP8266_SendATCmd("AT+CIPMUX=1\r\n","OK",100)) return 4;
    printf("ok.\n");

    printf("5.开启服务器........");
    if(ESP8266_SendATCmd("AT+CIPSERVER=1\r\n","OK",100)) return 5;
    printf("ok\n");

    printf("WIFI名称:%s\n", _pSSID);
    printf("WIFI密码:%s\n", _pPwd);
    printf("服务端IP地址:%s\n", ip);
    printf("服务端MAC地址:%s\n", mac);
    printf("服务端端口号:333[缺省]\n");
    printf("等待设备连接......\n");

    return 0;

}

/*
****************************************************************************************************************
   
    函数原型: u8 ESP8266_TCPClient_Init(char *_pSSID,char *_pPwd)
    函数功能: STA模式功能配置
    函数形参: 
            _pSSID WIFI名称
            _pPwd  WIFI密码
    硬件连接: 无
    返 回 值: 0成功/1失败
    
****************************************************************************************************************  
*/
u8 ESP8266_TCPClient_Init(char *_pSSID,char *_pPwd)
{
    char buf[100],ip[20],mac[20];
    char *pIP = NULL;
    char *pMAC = NULL;
    char *pEnd = NULL;
    
    printf("0.设备重启..........");
    USARTx_SendString(USART3, "AT+RST\r\n");
    delay_ms(1000);
    delay_ms(1000);
    printf("ok.\n");
    
    printf("1.配置为Staion模式......");
    if(ESP8266_SendATCmd("AT+CWMODE=1\r\n","OK",100)) return 1;
    printf("ok.\n");

    printf("2.连接到AP........");
    sprintf(buf,"AT+CWJAP_DEF=\"%s\",\"%s\"\r\n",_pSSID,_pPwd);
    if(ESP8266_SendATCmd(buf,"OK",1000)) return 2;
    printf("ok.\n"); 

    printf("3.查询本机IP地址....");
    if(ESP8266_SendATCmd("AT+CIFSR\r\n","OK",100)) return 3;
    //+CIFSR:STAIP,"192.168.4.1" 
    //+CIFSR:STAMAC,"a2:20:a6:19:c7:0a"
    pIP = strstr(g_ucUsart3Buff,"STAIP") + 7;
    pMAC = strstr(g_ucUsart3Buff,"STAMAC") +8;
    if((pIP != NULL) && (pMAC != NULL))
    {
        pEnd = strstr(pIP,"\"");//find "\"'s first appeared address
        strncpy(ip,pIP,pEnd-pIP);
        pEnd = NULL;
        pEnd = strstr(pMAC,"\"");
        strncpy(mac,pMAC,pEnd-pMAC);
    }
    printf("ok.\n");

    printf("4.开启单链接........");
    if(ESP8266_SendATCmd("AT+CIPMUX=0\r\n","OK",100)) return 4;
    printf("ok.\n");


    printf("WIFI名称:%s\n", _pSSID);
    printf("WIFI密码:%s\n", _pPwd);
    printf("客户端IP地址:%s\n",ip);
    printf("客户端MAC地址:%s\n",mac);

    return 0;
}

/*
****************************************************************************************************************
   
    函数原型: u8 ESP8266_Send2Client(char *_pData,u8 _ucLen)
    函数功能: 服务端向客户端发送数据
    函数形参: 
            _pData 要发送的数据
            _ucLen 数据长度
    硬件连接:
    返 回 值: 0-成功/1-失败

    1. 单连接时：(+CIPMUX=0) 
    AT+CIPSENDEX=<length>
    2. 多连接时：(+CIPMUX=1) 
    AT+CIPSENDEX=<link	ID>,<length>
    3. 如果是 UDP 传输，可以设置远端 IP 和端口：
    AT+CIPSENDEX=[<link	ID>,]<length>[,<remote	IP>,<remote	port>]
    指令功能：在普通传输模式时，设置发送数据的长度.
    收到此命令后先换行返回 >，然后开始接收串口数据，当数据长度满 length 或者遇到字符 \0	
    时，发送数据.
    如果未建立连接或连接被断开，返回：
    ERROR
    如果数据发送成功，返回：
    SEND OK
    如果数据发送失败，返回：
    SEND FAIL

****************************************************************************************************************  
*/
u8 ESP8266_SendData2Client(u16 _usLux,float _fTemp)
{
    char buff[50];
    char cmd[20];
    sprintf(buff,"*%05d,%.2f\n",_usLux,_fTemp);//拼接要发送的数据
    sprintf(cmd,"AT+CIPSENDEX=0,%d\r\n",strlen(buff));//多连接时设置

    if (ESP8266_SendATCmd(cmd,">",200) == 0)
    {
        if(ESP8266_SendATCmd(buff,"SEND OK",200) == 0)
            return 0;
    }
    return 1;
}

/*
****************************************************************************************************************
   
    函数原型: u8 ESP8266_SendData2Server(char *_usDat)
    函数功能: 向远端服务器发送信息
    函数形参: _usDat 要发送的信息
    硬件连接: 无
    返 回 值: 0成功/1失败
    
****************************************************************************************************************  
*/
u8 ESP8266_SendData2Server(char *_usDat,char *_pRemoteIP, u16 _usRemotePort)
{
    char data[50];
    /* 连接到 TCP 服务器 */
    sprintf(data, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", _pRemoteIP, _usRemotePort);
    if (ESP8266_SendATCmd(data, "OK", 1000))    return 1;
    
    sprintf(data,"AT+CIPSEND=%d\r\n",strlen(_usDat));
    if(ESP8266_SendATCmd(data,">",200)) return 2;
    
    if(ESP8266_SendATCmd(_usDat,"SEND OK",200)) return 3;
    
    return 0;
}

