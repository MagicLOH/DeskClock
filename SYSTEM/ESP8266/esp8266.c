#include "esp8266.h"

/*
****************************************************************************************************************
   
    ����ԭ��: static u8 ESP8266_SendATCmd(char *_pCmd,char *_pAck)
    ��������: ͨ������3 ��ESP8266 ����AT����ָ��
    �����β�: 
            _pCmd ATָ��
            _pAck �ظ���Ӧ��(OK��)
    Ӳ������: ��
    �� �� ֵ: �ɹ�-0/ʧ��-1
    
****************************************************************************************************************  
*/
static u8 ESP8266_SendATCmd(char *_pCmd,char *_pAck,u16 _usOvertime)
{
#if 0
    u16 count = 0;
    for(u8 i=0;i<5;i++)//��෢�����
    {
        g_ucUsart3Cnt = 0;
        g_ucUsart3RXFLAG = 0;
        USARTx_SendString(USART3,_pCmd);

        for(u8 j=0;j<3;j++)//�����������
        {
            while(g_ucUsart3RXFLAG == 0)
            {
                delay_ms(1);
                if(count++ % 50 == 0) LED2 = !LED2;
                if(_usOvertime == count) break;//g_ucUsart3RXFLAG = 1;
            }
            if(g_ucUsart3RXFLAG == 0) continue;//���RX��־λ��Ϊ0 ������һ��
            //g_ucUsart3RXFLAG = 0;
            if(strstr(g_ucUsart3Buff,_pAck) == NULL) ;//12 346""
            else 
            {
                LED2 = 1;//����
                g_ucUsart3Cnt = 0;
                g_ucUsart3RXFLAG = 0;
                return 0;//�ɹ�
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
    for(u8 i=0;i<5;i++)//��෢�����
    {
        g_ucUsart3Cnt = 0;
        g_ucUsart3RXFLAG = 0;
        USARTx_SendString(USART3,_pCmd);

        for(u8 j=0;j<3;j++)//�����������
        {
            count = 0;
            while(g_ucUsart3RXFLAG == 0)
            {
                delay_ms(1);
                count++;
                if(count % 50 == 0) LED2 = !LED2;
                if(_usOvertime == count) break;//g_ucUsart3RXFLAG = 1;
            }
            if(g_ucUsart3RXFLAG == 0) continue;//���RX��־λ��Ϊ0 ������һ��
            
            g_ucUsart3RXFLAG = 0;
            if(strstr(g_ucUsart3Buff,_pAck)) //12 346""
            {
                LED2 = 1;//����
                g_ucUsart3Cnt = 0;
                //g_ucUsart3RXFLAG = 0;
                return 0;//�ɹ�
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
   
    ����ԭ��: u8 ESP8266_TCPServer_Init(char *_pSSID,char *_pPwd)
    ��������: SoftAPģʽ��������
    �����β�: 
            _pSSID WIFI����
            _pPwd  WIFI����
    Ӳ������: ��
    �� �� ֵ: 0�ɹ�/1ʧ��
    
****************************************************************************************************************  
*/
u8 ESP8266_TCPServer_Init(char *_pSSID,char *_pPwd)
{
    char buf[50],ip[20],mac[20];
    char *pIP = NULL;
    char *pMAC = NULL;
    char *pEnd = NULL;
    
    printf("0.�豸����..........");
    USARTx_SendString(USART3, "AT+RST\r\n");
    delay_ms(1000);
    delay_ms(1000);
    printf("ok.\n");
    
    printf("1.����ΪAPģʽ......");
    if(ESP8266_SendATCmd("AT+CWMODE=2\r\n","OK",100)) return 1;
    printf("ok.\n");

    printf("2.����AP��Ϣ........");
    sprintf(buf,"AT+CWSAP_DEF=\"%s\",\"%s\",5,4\r\n",_pSSID,_pPwd);
    //if(ESP8266_SendATCmd(buf,"OK")) return 2;
    printf("ok.\n"); 

    printf("3.��ѯ����IP��ַ....");
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

    printf("4.����������........");
    if(ESP8266_SendATCmd("AT+CIPMUX=1\r\n","OK",100)) return 4;
    printf("ok.\n");

    printf("5.����������........");
    if(ESP8266_SendATCmd("AT+CIPSERVER=1\r\n","OK",100)) return 5;
    printf("ok\n");

    printf("WIFI����:%s\n", _pSSID);
    printf("WIFI����:%s\n", _pPwd);
    printf("�����IP��ַ:%s\n", ip);
    printf("�����MAC��ַ:%s\n", mac);
    printf("����˶˿ں�:333[ȱʡ]\n");
    printf("�ȴ��豸����......\n");

    return 0;

}

/*
****************************************************************************************************************
   
    ����ԭ��: u8 ESP8266_TCPClient_Init(char *_pSSID,char *_pPwd)
    ��������: STAģʽ��������
    �����β�: 
            _pSSID WIFI����
            _pPwd  WIFI����
    Ӳ������: ��
    �� �� ֵ: 0�ɹ�/1ʧ��
    
****************************************************************************************************************  
*/
u8 ESP8266_TCPClient_Init(char *_pSSID,char *_pPwd)
{
    char buf[100],ip[20],mac[20];
    char *pIP = NULL;
    char *pMAC = NULL;
    char *pEnd = NULL;
    
    printf("0.�豸����..........");
    USARTx_SendString(USART3, "AT+RST\r\n");
    delay_ms(1000);
    delay_ms(1000);
    printf("ok.\n");
    
    printf("1.����ΪStaionģʽ......");
    if(ESP8266_SendATCmd("AT+CWMODE=1\r\n","OK",100)) return 1;
    printf("ok.\n");

    printf("2.���ӵ�AP........");
    sprintf(buf,"AT+CWJAP_DEF=\"%s\",\"%s\"\r\n",_pSSID,_pPwd);
    if(ESP8266_SendATCmd(buf,"OK",1000)) return 2;
    printf("ok.\n"); 

    printf("3.��ѯ����IP��ַ....");
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

    printf("4.����������........");
    if(ESP8266_SendATCmd("AT+CIPMUX=0\r\n","OK",100)) return 4;
    printf("ok.\n");


    printf("WIFI����:%s\n", _pSSID);
    printf("WIFI����:%s\n", _pPwd);
    printf("�ͻ���IP��ַ:%s\n",ip);
    printf("�ͻ���MAC��ַ:%s\n",mac);

    return 0;
}

/*
****************************************************************************************************************
   
    ����ԭ��: u8 ESP8266_Send2Client(char *_pData,u8 _ucLen)
    ��������: �������ͻ��˷�������
    �����β�: 
            _pData Ҫ���͵�����
            _ucLen ���ݳ���
    Ӳ������:
    �� �� ֵ: 0-�ɹ�/1-ʧ��

    1. ������ʱ��(+CIPMUX=0) 
    AT+CIPSENDEX=<length>
    2. ������ʱ��(+CIPMUX=1) 
    AT+CIPSENDEX=<link	ID>,<length>
    3. ����� UDP ���䣬��������Զ�� IP �Ͷ˿ڣ�
    AT+CIPSENDEX=[<link	ID>,]<length>[,<remote	IP>,<remote	port>]
    ָ��ܣ�����ͨ����ģʽʱ�����÷������ݵĳ���.
    �յ���������Ȼ��з��� >��Ȼ��ʼ���մ������ݣ������ݳ����� length ���������ַ� \0	
    ʱ����������.
    ���δ�������ӻ����ӱ��Ͽ������أ�
    ERROR
    ������ݷ��ͳɹ������أ�
    SEND OK
    ������ݷ���ʧ�ܣ����أ�
    SEND FAIL

****************************************************************************************************************  
*/
u8 ESP8266_SendData2Client(u16 _usLux,float _fTemp)
{
    char buff[50];
    char cmd[20];
    sprintf(buff,"*%05d,%.2f\n",_usLux,_fTemp);//ƴ��Ҫ���͵�����
    sprintf(cmd,"AT+CIPSENDEX=0,%d\r\n",strlen(buff));//������ʱ����

    if (ESP8266_SendATCmd(cmd,">",200) == 0)
    {
        if(ESP8266_SendATCmd(buff,"SEND OK",200) == 0)
            return 0;
    }
    return 1;
}

/*
****************************************************************************************************************
   
    ����ԭ��: u8 ESP8266_SendData2Server(char *_usDat)
    ��������: ��Զ�˷�����������Ϣ
    �����β�: _usDat Ҫ���͵���Ϣ
    Ӳ������: ��
    �� �� ֵ: 0�ɹ�/1ʧ��
    
****************************************************************************************************************  
*/
u8 ESP8266_SendData2Server(char *_usDat,char *_pRemoteIP, u16 _usRemotePort)
{
    char data[50];
    /* ���ӵ� TCP ������ */
    sprintf(data, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", _pRemoteIP, _usRemotePort);
    if (ESP8266_SendATCmd(data, "OK", 1000))    return 1;
    
    sprintf(data,"AT+CIPSEND=%d\r\n",strlen(_usDat));
    if(ESP8266_SendATCmd(data,">",200)) return 2;
    
    if(ESP8266_SendATCmd(_usDat,"SEND OK",200)) return 3;
    
    return 0;
}

