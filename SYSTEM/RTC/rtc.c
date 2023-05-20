#include "rtc.h"

static u8 Year_judge(u16 _usyear);
struct rtc_clock Clock_Calendar;//最后的标准时间格式输出结构体变量
static const struct rtc_clock g_tRTC_BaseTime={1970,1,1,0,0,0,0};//年,月,日,时,分,秒,周
static const u8 A_ucMonthDay[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void RTC_SecToStd(u32 _ulTotalSec);
static u32 RTC_StdToSec(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek);
void RTC_SetStdTime(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek);
static u8 RTC_Calculate_Week(u16 _usYear ,u8 _ucMonth,u8 _ucDay);
/*配置过程：
        1. 查询RTOFF位，直到RTOFF的值变为'1'
        2. 置CNF值为1，进入配置模式
        3. 对一个或多个RTC寄存器进行写操作
        4. 清除CNF标志位，退出配置模式
        5. 查询RTOFF，直至RTOFF位变为'1'以确认写操作已经完成.
        仅当CNF标志位被清除时，写操作才能进行，这个过程至少需要3个RTCCLK周期.*/
/*
****************************************************************************************************************
   
    函数原型: void RTC_Initial(void )
    函数功能: RTC 外部时钟使能
    函数形参: 无
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void RTC_Init(void )
{
    /*时钟使能并开启备份域写权限*/
    RCC->APB1ENR |= 1<<28;//电源接口时钟使能 (Power interface clock enable)
    RCC->APB1ENR |= 1<<27;//备份接口时钟使能 (Backup interface clock enable)
    PWR->CR |= 1<<8;      //允许写入RTC和后备寄存器
    /*复位备份域*/
    RCC->BDCR |= 1<<16;   //复位
    RCC->BDCR &= ~(1<<16);//结束
    /*选择不同(LSE/HSE/LSI暂不考虑)时钟信号*/
    RCC->BDCR |= 1<<0;    //LSE振荡器时钟使能
    u8 Errtime = 0;
    do
    {   
        delay_ms(1);
        Errtime++;
        if(Errtime == 0xFE) break;
    } while (!(RCC->BDCR & 1<<1));//直到外部低速LSE就绪
    if(Errtime != 0xFE)           //LSE ready!
    {
        RCC->BDCR |= 1<<8;//选择LSE振荡器作为RTC时钟
    }
    else
    {
        //RCC->BDCR |= 1<<16;//复位整个备份域
        RCC->BDCR &= ~(1<<0);//LSE振荡器失能
        RCC->BDCR |= 0x3<<8; //开启HSE振荡器在128分频后作为RTC时钟。
    }
    RCC->BDCR |= 1<<15;//RTC时钟使能
    
    /*核心配置 外设功能*/
    while(!(RTC->CRL & 1<<5));//等待上一次对RTC寄存器的写操作已经完成
    RTC->CRL |= 1<<4;//进入配置模式!
        /*闹钟配置*/
    RTC->ALRH |= 0;
    RTC->ALRL |= 30;
        /*计数器配置*/
    RTC->CNTH |= 0;
    RTC->CNTL |= 0;
        /*预分频装载寄存器配置*/
    RTC->PRLH |= 0;
    if(Errtime != 0xFE)//是LSE时钟
        RTC->PRLL = 0x7FFF;//0x7FFF --> 32768D=7FFFH  -->cnt+1 = 1s
    else//是HSE
        RTC->PRLL = 0xF423;//0xF423 --> 0xF424 = 62500D = 8*1000*1000 / 128分频
    RTC->CRL &= ~(1<<4);//退出配置模式! 开始对ALR,CNT,PRL写操作
    while(!(RTC->CRL & 1<<5));//等待上一次对RTC寄存器的写操作已经完成
    
    /*RTC相关中断配置*/
    RTC->CRH |= 1<<0;//开启秒中断
    CM3_NVIC_SetPriority(RTC_IRQn,1,1);
    
}
/*
****************************************************************************************************************
   
    函数原型: void RTC_IRQHandler(void)
    函数功能: RTC 中断服务例程
    函数形参: 无
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void RTC_IRQHandler(void)
{
    if(RTC->CRL & 1<<0)//秒中断标志位成立
    {
        RTC_SecToStd(RTC->CNTH <<16 | RTC->CNTL);//补全为32位的最终结果送进 结构体Clock_Calendar。
        //printf("%04d/%02d/%02d %02d:%02d:%02d 星期%d %.1f摄氏度\n",Clock_Calendar.usYear,\
                                                        Clock_Calendar.ucMonth,\
                                                        Clock_Calendar.ucDay,\
                                                        Clock_Calendar.ucHour,\
                                                        Clock_Calendar.ucMin,\
                                                        Clock_Calendar.ucSec,\
                                                        Clock_Calendar.ucWeek,\
                                                        DS18B20_GetTemperature());
    }
    RTC->CRL &= ~(1<<0);//清除秒中断标志
}

/*
****************************************************************************************************************
   
    函数原型: u8 Year_judge(u16 _usyear)
    函数功能: 计算年份是闰年(leap year)还是平年(common year)
    函数形参: u16 _usyear
    硬件连接: 无
    返 回 值: 无
    
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
   
    函数原型: u8 Calculate_Week(u16 _ucYear ,u8 _ucMonth,u8 _ucDay)
    函数功能: 计算任意一年的星期几
    函数形参: u16 _ucYear ,u8 _ucMonth,u8 _ucDay
    硬件连接: 无
    返 回 值: 星期几
    
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
   
    函数原型: static u32 RTC_StdToSec(u16 usYear,u8 ucMonth,u8 ucDay,u8 ucHour,u8 ucMin,u8 ucSec,u8 ucWeek)
    函数功能: 标准时间(基于某个特定时间)转秒单位时间
    函数形参: u16 usYear,u8 ucMonth,u8 ucDay,u8 ucHour,u8 ucMin,u8 ucSec,u8 ucWeek
    硬件连接: 无
    返 回 值: u32 ulTotalSec 转换后的总秒时间
    时间基准: 2000/01/01 0:0:0
****************************************************************************************************************  
*/
static u32 RTC_StdToSec(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek)
{
    u32 ulTotalSec = 0;
    u16 i;
    //转换年
    for(i=g_tRTC_BaseTime.usYear;i<_usYear;i++)
    {
        if(Year_judge(i))//true leap year 
            ulTotalSec += 366*24*60*60;
        else//flase common year
            ulTotalSec += 365*24*60*60;
    }
    //转换月
    for(i=g_tRTC_BaseTime.ucMonth;i<_ucMonth;i++)
    {
        ulTotalSec += A_ucMonthDay[i]*24*60*60;
        if(i==2 && Year_judge(_usYear))//如果是闰年的2月份
            ulTotalSec += 24*60*60;//再多加一天
    }
    //转换日
    ulTotalSec += (_ucDay-1)*24*60*60;
    //转换时
    ulTotalSec += _ucHour*60*60;
    //转换分
    ulTotalSec += _ucMin*60;
    //转换秒
    ulTotalSec += _ucSec;

    return ulTotalSec;
}

/*
****************************************************************************************************************
   
    函数原型: static void RTC_SecToStd(u32 _ulTotalSec)
    函数功能: 秒时间(转换好的基于某时刻的时间)转标准时间
    函数形参: u32 _ulTotalSec
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void RTC_SecToStd(u32 _ulTotalSec)
{
    Clock_Calendar = g_tRTC_BaseTime;
    //转换年
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
        Clock_Calendar.usYear++;//_ulTotalSec逐年减去直至不够减-->dec month begin
    }
    //转换月
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
        Clock_Calendar.ucMonth++;//_ulTotalSec逐月减去直至不够减-->dec day begin
    }
    //转换日
    while(_ulTotalSec >= 24*60*60)
    {
        _ulTotalSec -= 24*60*60;
        Clock_Calendar.ucDay++;
    }
    //转换时
    while(_ulTotalSec >= 60*60)
    {
        _ulTotalSec -= 60*60;
        Clock_Calendar.ucHour++;
    }
    //转换分
    while(_ulTotalSec >= 60)
    {
        _ulTotalSec -= 60;
        Clock_Calendar.ucMin++;
    }
    //转换秒
    Clock_Calendar.ucSec = _ulTotalSec;
    Clock_Calendar.ucWeek =RTC_Calculate_Week(Clock_Calendar.usYear,Clock_Calendar.ucMonth,Clock_Calendar.ucDay);
}

/*
****************************************************************************************************************
   
    函数原型: void RTC_SetStdTime(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek)
    函数功能: 设置当前时间
    函数形参: u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void RTC_SetStdTime(u16 _usYear,u8 _ucMonth,u8 _ucDay,u8 _ucHour,u8 _ucMin,u8 _ucSec,u8 _ucWeek)
{
    u32 ulTotalSEC = RTC_StdToSec(_usYear,_ucMonth,_ucDay,_ucHour,_ucMin,_ucSec,_ucWeek);
    /*开启备份域写权限*/
    RCC->APB1ENR |= 1<<27;//备份接口时钟使能
    RCC->APB1ENR |= 1<<28;//电源接口备份使能
    PWR->CR |= 1<<8;//取消后被区域写保护

    /*对RTC寄存器开始写操作*/
    while(!(RTC->CRL & 1 <<5));//等待上一次对RTC寄存器的写操作已经完成
    RTC->CRL |= 1<<4;//进入配置模式

    RTC->CNTH = ulTotalSEC >>16;
    RTC->CNTL = ulTotalSEC;

    RTC->CRL &= ~(1<<4);//退出配置模式
    while(!(RTC->CRL & 1 <<5));//等待上一次对RTC寄存器的写操作已经完成(可加可不加)
    
}

/*
****************************************************************************************************************
   
    函数原型:
    函数功能:
    函数形参:
    硬件连接:
    返 回 值:
    
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
                        Clock_Calendar.ucWeek);//再次设置标准时间
    }
}
        

