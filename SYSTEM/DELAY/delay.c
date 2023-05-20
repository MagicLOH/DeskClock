#include "delay.h"

/*
****************************************************************************************************************
    
    函数原型：void delay_us(u32 us) void delay_ms(u16 ms) void delay_s(u16 s)
    函数功能：实现精准延时功能,以微秒/毫秒/秒为单位
    函数形参：u32 us ,u16 ms, u16 s
    硬件连接：无
    返回值：无
    
****************************************************************************************************************  
*/
void delay_us(u32 _us)  //us 最大值为233015
{
    SysTick->LOAD=72*_us;                //重装计数初值（当主频为72Mhz，72次为1us）
    SysTick->VAL=0x00;                  //清空定时器的计数器
    SysTick->CTRL=0x5<<0;               //第 0 位置 1 打开定时器 第 2 位表示时钟源-->内核时钟源
    while(!( SysTick->CTRL & (1<<16) ));//第 16 为表示当计数器计数到 0
    SysTick->CTRL &= ~(1<<0);           //第 0 位清零 关闭定时器 第 2 位表示时钟源-->内核时钟源
}
// void delay_us(u32 _us) 
// {
//     SysTick->CTRL &= ~(1<<2);           //选择外部时钟 9Mhz    
//     SysTick->LOAD = 9*_us;              //重装计数初值（当主频为72Mhz，72次为1us）
//     SysTick->VAL  = 0x00;               //清空定时器的计数器
//     SysTick->CTRL |= 1<<0;              //第 0 位置 1 打开定时器 第 2 位表示时钟源-->内核时钟源
//     while(!( SysTick->CTRL & (1<<16) ));//第 16 为表示当计数器计数到 0
//     SysTick->CTRL &= ~(1<<0);           //第 0 位清零 关闭定时器 第 2 位表示时钟源-->内核时钟源
// }
void delay_ms(u16 _ms)
{
    while( _ms-- != 0 )
        delay_us(1000);
}
void delay_s(u16 _s)
{
    while( _s-- != 0 )
        delay_ms(1000);
}