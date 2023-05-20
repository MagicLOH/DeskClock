#include "rtc.h"

static u8 Year_judge(u16 _usyear);
struct rtc_clock Clock_Calendar;//���ı�׼ʱ���ʽ����ṹ�����
static const struct rtc_clock g_tRTC_BaseTime={1970,1,1,0,0,0,0};//��,��,��,ʱ,��,��,��
static const u8 A_ucMonthDay[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void RTC_SecToStd(u32 _ulTotalSec);
static u32 RTC_StdToSec(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek);
void RTC_SetStdTime(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek);
static u8 RTC_Calculate_Week(u16 _usYear ,u8 _ucMonth,u8 _ucDay);
/*���ù��̣�
        1. ��ѯRTOFFλ��ֱ��RTOFF��ֵ��Ϊ'1'
        2. ��CNFֵΪ1����������ģʽ
        3. ��һ������RTC�Ĵ�������д����
        4. ���CNF��־λ���˳�����ģʽ
        5. ��ѯRTOFF��ֱ��RTOFFλ��Ϊ'1'��ȷ��д�����Ѿ����.
        ����CNF��־λ�����ʱ��д�������ܽ��У��������������Ҫ3��RTCCLK����.*/
/*
****************************************************************************************************************
   
    ����ԭ��: void RTC_Initial(void )
    ��������: RTC �ⲿʱ��ʹ��
    �����β�: ��
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void RTC_Init(void )
{
    /*ʱ��ʹ�ܲ�����������дȨ��*/
    RCC->APB1ENR |= 1<<28;//��Դ�ӿ�ʱ��ʹ�� (Power interface clock enable)
    RCC->APB1ENR |= 1<<27;//���ݽӿ�ʱ��ʹ�� (Backup interface clock enable)
    PWR->CR |= 1<<8;      //����д��RTC�ͺ󱸼Ĵ���
    /*��λ������*/
    RCC->BDCR |= 1<<16;   //��λ
    RCC->BDCR &= ~(1<<16);//����
    /*ѡ��ͬ(LSE/HSE/LSI�ݲ�����)ʱ���ź�*/
    RCC->BDCR |= 1<<0;    //LSE����ʱ��ʹ��
    u8 Errtime = 0;
    do
    {   
        delay_ms(1);
        Errtime++;
        if(Errtime == 0xFE) break;
    } while (!(RCC->BDCR & 1<<1));//ֱ���ⲿ����LSE����
    if(Errtime != 0xFE)           //LSE ready!
    {
        RCC->BDCR |= 1<<8;//ѡ��LSE������ΪRTCʱ��
    }
    else
    {
        //RCC->BDCR |= 1<<16;//��λ����������
        RCC->BDCR &= ~(1<<0);//LSE����ʧ��
        RCC->BDCR |= 0x3<<8; //����HSE������128��Ƶ����ΪRTCʱ�ӡ�
    }
    RCC->BDCR |= 1<<15;//RTCʱ��ʹ��
    
    /*�������� ���蹦��*/
    while(!(RTC->CRL & 1<<5));//�ȴ���һ�ζ�RTC�Ĵ�����д�����Ѿ����
    RTC->CRL |= 1<<4;//��������ģʽ!
        /*��������*/
    RTC->ALRH |= 0;
    RTC->ALRL |= 30;
        /*����������*/
    RTC->CNTH |= 0;
    RTC->CNTL |= 0;
        /*Ԥ��Ƶװ�ؼĴ�������*/
    RTC->PRLH |= 0;
    if(Errtime != 0xFE)//��LSEʱ��
        RTC->PRLL = 0x7FFF;//0x7FFF --> 32768D=7FFFH  -->cnt+1 = 1s
    else//��HSE
        RTC->PRLL = 0xF423;//0xF423 --> 0xF424 = 62500D = 8*1000*1000 / 128��Ƶ
    RTC->CRL &= ~(1<<4);//�˳�����ģʽ! ��ʼ��ALR,CNT,PRLд����
    while(!(RTC->CRL & 1<<5));//�ȴ���һ�ζ�RTC�Ĵ�����д�����Ѿ����
    
    /*RTC����ж�����*/
    RTC->CRH |= 1<<0;//�������ж�
    CM3_NVIC_SetPriority(RTC_IRQn,1,1);
    
}
/*
****************************************************************************************************************
   
    ����ԭ��: void RTC_IRQHandler(void)
    ��������: RTC �жϷ�������
    �����β�: ��
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void RTC_IRQHandler(void)
{
    if(RTC->CRL & 1<<0)//���жϱ�־λ����
    {
        RTC_SecToStd(RTC->CNTH <<16 | RTC->CNTL);//��ȫΪ32λ�����ս���ͽ� �ṹ��Clock_Calendar��
        //printf("%04d/%02d/%02d %02d:%02d:%02d ����%d %.1f���϶�\n",Clock_Calendar.usYear,\
                                                        Clock_Calendar.ucMonth,\
                                                        Clock_Calendar.ucDay,\
                                                        Clock_Calendar.ucHour,\
                                                        Clock_Calendar.ucMin,\
                                                        Clock_Calendar.ucSec,\
                                                        Clock_Calendar.ucWeek,\
                                                        DS18B20_GetTemperature());
    }
    RTC->CRL &= ~(1<<0);//������жϱ�־
}

/*
****************************************************************************************************************
   
    ����ԭ��: u8 Year_judge(u16 _usyear)
    ��������: �������������(leap year)����ƽ��(common year)
    �����β�: u16 _usyear
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
static u8 Year_judge(u16 _usyear)
{
    if((_usyear %4 == 0 && _usyear %100 != 0)|| _usyear %400 ==0)
        return leap_year;
    else 
        return common_year;
}

/*
****************************************************************************************************************
   
    ����ԭ��: u8 Calculate_Week(u16 _ucYear ,u8 _ucMonth,u8 _ucDay)
    ��������: ��������һ������ڼ�
    �����β�: u16 _ucYear ,u8 _ucMonth,u8 _ucDay
    Ӳ������: ��
    �� �� ֵ: ���ڼ�
    
****************************************************************************************************************  
*/
static u8 RTC_Calculate_Week(u16 _usYear ,u8 _ucMonth,u8 _ucDay)
{
	int c,y,week;
	if (_ucMonth == 1 || _ucMonth == 2)
        _usYear--, _ucMonth += 12;
    c = _usYear / 100;
    y = _usYear - c * 100;
    week = y + y/4 + c/4 - 2*c + 26 * (_ucMonth + 1)/10 + _ucDay-1;
	while (week < 0)
        week += 7;
    week %= 7;
	return week;
}

/*
****************************************************************************************************************
   
    ����ԭ��: static u32 RTC_StdToSec(u16 usYear,u8 ucMonth,u8 ucDay,u8 ucHour,u8 ucMin,u8 ucSec,u8 ucWeek)
    ��������: ��׼ʱ��(����ĳ���ض�ʱ��)ת�뵥λʱ��
    �����β�: u16 usYear,u8 ucMonth,u8 ucDay,u8 ucHour,u8 ucMin,u8 ucSec,u8 ucWeek
    Ӳ������: ��
    �� �� ֵ: u32 ulTotalSec ת���������ʱ��
    ʱ���׼: 2000/01/01 0:0:0
****************************************************************************************************************  
*/
static u32 RTC_StdToSec(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek)
{
    u32 ulTotalSec = 0;
    u16 i;
    //ת����
    for(i=g_tRTC_BaseTime.usYear;i<_usYear;i++)
    {
        if(Year_judge(i))//true leap year 
            ulTotalSec += 366*24*60*60;
        else//flase common year
            ulTotalSec += 365*24*60*60;
    }
    //ת����
    for(i=g_tRTC_BaseTime.ucMonth;i<_ucMonth;i++)
    {
        ulTotalSec += A_ucMonthDay[i]*24*60*60;
        if(i==2 && Year_judge(_usYear))//����������2�·�
            ulTotalSec += 24*60*60;//�ٶ��һ��
    }
    //ת����
    ulTotalSec += (_ucDay-1)*24*60*60;
    //ת��ʱ
    ulTotalSec += _ucHour*60*60;
    //ת����
    ulTotalSec += _ucMin*60;
    //ת����
    ulTotalSec += _ucSec;

    return ulTotalSec;
}

/*
****************************************************************************************************************
   
    ����ԭ��: static void RTC_SecToStd(u32 _ulTotalSec)
    ��������: ��ʱ��(ת���õĻ���ĳʱ�̵�ʱ��)ת��׼ʱ��
    �����β�: u32 _ulTotalSec
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void RTC_SecToStd(u32 _ulTotalSec)
{
    Clock_Calendar = g_tRTC_BaseTime;
    //ת����
    while(1)
    {
        if(Year_judge(Clock_Calendar.usYear))//true --> leap year
        {
            if(_ulTotalSec >= 366*24*60*60)
                _ulTotalSec -= 366*24*60*60;
            else break;
        }
        else //flase --> common year
        {
            if(_ulTotalSec >= 365*24*60*60)
                _ulTotalSec -= 365*24*60*60;
            else break;
        }
        Clock_Calendar.usYear++;//_ulTotalSec�����ȥֱ��������-->dec month begin
    }
    //ת����
    while(1)
    {
        if(_ulTotalSec >= A_ucMonthDay[Clock_Calendar.ucMonth]*24*60*60)
            _ulTotalSec -= A_ucMonthDay[Clock_Calendar.ucMonth]*24*60*60;
        else break;
        if(A_ucMonthDay[Clock_Calendar.ucMonth]==2 && Year_judge(Clock_Calendar.usYear))
        {
            if(_ulTotalSec >=24*60*60)
                _ulTotalSec -=24*60*60;
        }
        Clock_Calendar.ucMonth++;//_ulTotalSec���¼�ȥֱ��������-->dec day begin
    }
    //ת����
    while(_ulTotalSec >= 24*60*60)
    {
        _ulTotalSec -= 24*60*60;
        Clock_Calendar.ucDay++;
    }
    //ת��ʱ
    while(_ulTotalSec >= 60*60)
    {
        _ulTotalSec -= 60*60;
        Clock_Calendar.ucHour++;
    }
    //ת����
    while(_ulTotalSec >= 60)
    {
        _ulTotalSec -= 60;
        Clock_Calendar.ucMin++;
    }
    //ת����
    Clock_Calendar.ucSec = _ulTotalSec;
    Clock_Calendar.ucWeek =RTC_Calculate_Week(Clock_Calendar.usYear,Clock_Calendar.ucMonth,Clock_Calendar.ucDay);
}

/*
****************************************************************************************************************
   
    ����ԭ��: void RTC_SetStdTime(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek)
    ��������: ���õ�ǰʱ��
    �����β�: u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void RTC_SetStdTime(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek)
{
    u32 ulTotalSEC = RTC_StdToSec(_usYear,_ucMonth,_ucDay,_ucHour,_ucMin,_ucSec,_ucWeek);
    /*����������дȨ��*/
    RCC->APB1ENR |= 1<<27;//���ݽӿ�ʱ��ʹ��
    RCC->APB1ENR |= 1<<28;//��Դ�ӿڱ���ʹ��
    PWR->CR |= 1<<8;//ȡ��������д����

    /*��RTC�Ĵ�����ʼд����*/
    while(!(RTC->CRL & 1 <<5));//�ȴ���һ�ζ�RTC�Ĵ�����д�����Ѿ����
    RTC->CRL |= 1<<4;//��������ģʽ

    RTC->CNTH = ulTotalSEC >>16;
    RTC->CNTL = ulTotalSEC;

    RTC->CRL &= ~(1<<4);//�˳�����ģʽ
    while(!(RTC->CRL & 1 <<5));//�ȴ���һ�ζ�RTC�Ĵ�����д�����Ѿ����(�ɼӿɲ���)
    
}

/*
****************************************************************************************************************
   
    ����ԭ��:
    ��������:
    �����β�:
    Ӳ������:
    �� �� ֵ:
    
****************************************************************************************************************  
*/
void RTC_Renew_Time(char *_pStdTime)
{
    if(_pStdTime[0]=='*' && strlen(_pStdTime)==15)
    {
        Clock_Calendar.usYear = (_pStdTime[1]-'0')*1000 + (_pStdTime[2]-'0')*100\
                              + (_pStdTime[3]-'0')*10   + (_pStdTime[4]-'0');
        Clock_Calendar.ucMonth= (_pStdTime[5]-'0')*10   + (_pStdTime[6]-'0');
        Clock_Calendar.ucDay  = (_pStdTime[7]-'0')*10   + (_pStdTime[8]-'0');
        Clock_Calendar.ucHour = (_pStdTime[9]-'0')*10   + (_pStdTime[10]-'0');
        Clock_Calendar.ucMin  = (_pStdTime[11]-'0')*10  + (_pStdTime[12]-'0');
        Clock_Calendar.ucSec  = (_pStdTime[13]-'0')*10  + (_pStdTime[14]-'0');
        
        RTC_SetStdTime( Clock_Calendar.usYear,\
                        Clock_Calendar.ucMonth,\
                        Clock_Calendar.ucDay,\
                        Clock_Calendar.ucHour,\
                        Clock_Calendar.ucMin,\
                        Clock_Calendar.ucSec,\
                        Clock_Calendar.ucWeek);//�ٴ����ñ�׼ʱ��
    }
}
        

