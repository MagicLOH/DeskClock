#include "oled.h"

/*建立缓存*/
u8 OLED_GRAM [OLED_Height/8][OLED_Width];
static void OLED_WR_Byte(u8 ,u8 );
static void OLED_SetPos(u8 _ucPage,u8 _ucCol);
static void OLED_ClearScreen(void);
static void OLED_DisplayDAT(u8 _ucPage,u8 _ucCol,u8 _ucWidth,u8 _ucHeight,const u8 *_pData);
/*
****************************************************************************************************************
   
    函数原型: void OLED_Initial(void)
    函数功能: OLED 显示屏初始化
    函数形参: 无
    硬件连接: 
             D0  -- PB14 -- SPI时钟线 -- OLED_SCLK -- 通用推挽输出模式
             D1  -- PB13 -- SPI数据线 -- OLED_MOSI -- 通用推挽输出模式
             RES#-- PB12 -- 复位线    -- OLED_RES -- 通用推挽输出模式
             DC# -- PB1  -- DATA/CMMAND -- OLED_DC --通用推挽输出模式
             CS# -- PA7  -- SPI片选信号线(不能悬空) -- OLED_CS --通用推挽输出模式
    返 回 值: 无
    
****************************************************************************************************************  
*/
void OLED_Init(void)
{
    /*GPIO相关配置*/
    RCC->APB2ENR |= 0x3<<2;  //GPIOA/B端口时钟使能
    //PB1/12/13/14,PA7配置为通用推挽输出
    GPIOA->CRL &= 0x0FFFFFFF;
    GPIOA->CRL |= 0X30000000;// CS
    GPIOB->CRL &= 0xFFFFFF0F;
    GPIOB->CRL |= 0x00000030;// DC
    GPIOB->CRH &= 0xF000FFFF;
    GPIOB->CRH |= 0x03330000;// D0,D1,RES
    //整体拉高电平
    GPIOA->ODR |= 1<<7;
    GPIOB->ODR |= 1<<1;
    GPIOB->ODR |= 0x7<<12;
    /*OLED相关配置*/
    //OLED硬件复位
    OLED_RES = 0;//开始
    delay_ms(100);
    OLED_RES = 1;//结束
    delay_ms(100);

    //初始化序列
    OLED_WR_Byte(0xAE, OLED_CMD);//--关闭显示
    OLED_WR_Byte(0x00, OLED_CMD);//--set low column address
    OLED_WR_Byte(0x10, OLED_CMD);//--set high column address
    OLED_WR_Byte(0x40, OLED_CMD);//--set start line address
    OLED_WR_Byte(0x81, OLED_CMD);//--设置屏幕对比度寄存器寄存器
    OLED_WR_Byte(0xCF, OLED_CMD);//--设置对比度[0x00~0xFF]
    OLED_WR_Byte(0xA1, OLED_CMD);//--Set SEG/Column Mapping 0xa0 左右反置 0xa1 正常
    OLED_WR_Byte(0xC8, OLED_CMD);//--Set COM/Row Scan Direction 0xc0 上下反置 0xc8 正常
    OLED_WR_Byte(0xA6, OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8, OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f, OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3, OLED_CMD);//--set display offset
    OLED_WR_Byte(0x00, OLED_CMD);//--not offset
    OLED_WR_Byte(0xd5, OLED_CMD);//--set display clock divide ratio/oscillatorfrequency
    OLED_WR_Byte(0x80, OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9, OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1, OLED_CMD);//--Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA, OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40, OLED_CMD);//--Set VCOM Deselect Level
    OLED_WR_Byte(0x20, OLED_CMD);//--设置寻址方式 (0x00/0x01/0x02)
    OLED_WR_Byte(0x02, OLED_CMD);//--页面寻址模式(页内列地址自增)
    OLED_WR_Byte(0x8D, OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14, OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4, OLED_CMD);//--Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6, OLED_CMD);//--设置显示模式(0Xa6 正常模式亮为 1/0xa7 反显模式亮为 0)
    OLED_WR_Byte(0xAF, OLED_CMD);//--开启显示

    OLED_ClearScreen();
}

/*
****************************************************************************************************************
   
    函数原型: static void OLED_WR_Byte(u8 _ucTxd,u8 _ucMode)
    函数功能: 利用SPI通信对OLED写入数据
    函数形参: u8 _ucTxd 待写入的数据,u8 _ucMode 写入命令(0)/数据(1)
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_WR_Byte(u8 _ucTxd,u8 _ucMode)
{
    OLED_CS = 0;      //开启片选
    OLED_DC = _ucMode;//写命令还是写数据
    OLED_SCLK = 0;    //闲置电平
    for(u8 i=0;i<8;i++)//写入一个字节
    {
        OLED_SCLK = 0;//等待上升沿
        if(_ucTxd & 0x80)
            OLED_MOSI = 1;
        else
            OLED_MOSI = 0;
        OLED_SCLK = 1;//给予高电平上升沿
        _ucTxd<<=1;   //移给下一位
    }
    OLED_SCLK = 0;//闲置电平
    OLED_CS = 1;  //关闭片选
}

/*
****************************************************************************************************************
   
    函数原型: static void OLED_SetPos(u8 _ucPage,u8 _ucCol,u8 _ucMode)
    函数功能: 设置写入数据起始位置
    函数形参: u8 _ucPage,u8 _ucCol
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_SetPos(u8 _ucPage,u8 _ucCol)
{
    //xx & xx表示取出第n位,设置参数保护
    OLED_WR_Byte(0xB0 + (_ucPage & 0x07),OLED_CMD);
    OLED_WR_Byte(0x00 + (_ucCol & 0x0F),OLED_CMD);      //低四位Set the lower start column address
    OLED_WR_Byte(0x10 + ((_ucCol & 0xF0)>>4),OLED_CMD); //高四位Set the upper start column address
}

/*
****************************************************************************************************************
   
    函数原型: static void OLED_ClearScreen(void)
    函数功能: OLED显示器清屏
    函数形参: 无
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_ClearScreen(void)
{
    for(u8 i=0;i<8;i++)
    {
        OLED_SetPos(i,0);
        for(u8 j=0;j<128;j++)
        {
            OLED_WR_Byte(0x00,OLED_DAT);
        }
    }
}

/*
****************************************************************************************************************
   
    函数原型: static void OLED_DisplayDAT(u8 _ucPage,u8 _ucCol,u8 _ucWidth,u8 _ucHeight,const u8 *_pData)
    函数功能: 在屏幕上显示数据
    函数形参: u8 _ucPage,u8 _ucCol,u8 _ucWidth,u8 _ucHeight,const u8 *_pData
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_DisplayDAT(u8 _ucPage,u8 _ucCol,u8 _ucWidth,u8 _ucHeight,const u8 *_pData)
{
    while(_ucHeight % 8) _ucHeight++;//保证行数一致
    for(u8 i=0;i<_ucHeight/8;i++)
    {
        OLED_SetPos(_ucPage+i,_ucCol);
        for(u8 j=0;j<_ucWidth;j++)
        {
            //OLED_WR_Byte(*_pData++,OLED_DAT);
            OLED_WR_Byte(_pData[j+i*_ucWidth],OLED_DAT);
        }
    }
}

/*
****************************************************************************************************************
   
    函数原型: void Display_CHN(u8 _ucPage,u8 _ucCol,u8 _ucSize,u8 _ucNum)
    函数功能: 显示汉字
    函数形参: u8 _ucPage,u8 _ucCol,u8 _ucSize,u8 _ucNum 在字库中的编号
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void Display_CHN(u8 _ucPage,u8 _ucCol,u8 _ucSize,u8 _ucNum,u8 _ucType)
{
    switch(_ucType)
    {
        default:
        OLED_DisplayDAT(_ucPage,_ucCol,_ucSize,_ucSize,Chinese16x16[_ucNum]);
        case 24*24/18:
        OLED_DisplayDAT(_ucPage,_ucCol,_ucSize,_ucSize,Chinese24x24[_ucNum]);
    }
    
}

/*
****************************************************************************************************************
   
    函数原型: void Display_String(u8 _ucPage,u8 _ucCol,u8 _ucHeight,char *_pStr,u8 _ucType)
    函数功能: 显示字符串
    函数形参: 
            u8 _ucPage,
            u8 _ucCol,
            u8 _ucHeight,
            char *_pStr,
            u8 _ucType
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void Display_String(u8 _ucPage,u8 _ucCol,u8 _ucHeight,char *_pStr,u8 _ucType)
{
    while(*_pStr != '\0')
    {
        /*自动换行*/
        if(_ucCol > 127)
        {
            _ucPage += _ucHeight/8;
            _ucCol = 0;
        }
        switch(_ucType)
        {
            case 6*8/8:
            OLED_DisplayDAT(_ucPage,_ucCol,_ucHeight-2,_ucHeight,Ascii6x8[*_pStr++ -' ']);
            break;
            default:
            OLED_DisplayDAT(_ucPage,_ucCol,_ucHeight/2,_ucHeight,Ascii8x16[*_pStr++ -' ']);
            break;
        }
        
        _ucCol += _ucHeight/2;//偏移列地址
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
****************************************************************************************************************
   
    函数原型: void OLED_DrawPoint(u8 _ucX,u8 _ucY,u8 _ucState)
    函数功能: 向缓存区GRAM画点
    函数形参: u8 _ucX,u8 _ucY,u8 _ucState 写入状态 亮1/灭0
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_DrawPoint(u8 _ucX,u8 _ucY,u8 _ucState)
{
    if(_ucX < OLED_Width && _ucY < OLED_Height)
    {
        u8 ucPage = _ucY / 8;
        u8 ucCol = _ucX;
        u8 ucPos = _ucY % 8;
        //if(ucPage < OLED_Height/8 && ucCol<127)
        if(_ucState)
            OLED_GRAM[ucPage][ucCol] |= 1<<ucPos;
        else 
            OLED_GRAM[ucPage][ucCol] &= ~(1<<ucPos);  
    }
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_DrawData_by_Col(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
    函数功能: [逐列式式]向GRAM写入数据
    函数形参:
            u8 _ucX 点横坐标
            u8 _ucY 点纵坐标
            u8 _ucWidth 取模数据宽度
            u8 _ucHeight 取模数据高度
            const u8 *_pData 取模数据
            u8 _ucMode 正常0/反显1 
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_DrawData_by_Col(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
{
    u8 ucY0 = _ucY;//记录Y的起始位置
    while(_ucHeight % 8) _ucHeight++;//若行数有余
    for(u8 i=0;i<(_ucWidth*_ucHeight/8);i++)
    {
        u8 ucByte = _pData[i];//取 1 byte数据
        for(u8 j=0;j<8;j++)//刷入 1 byte
        {
            if(!_ucMode)//Normal mode
                {
                    if( ucByte & 0x01)
                        OLED_DrawPoint(_ucX,_ucY,1);
                    else
                        OLED_DrawPoint(_ucX,_ucY,0);
                }
            else        //Abnormal mode
                {
                    if( ucByte & 0x01)
                        OLED_DrawPoint(_ucX,_ucY,0);
                    else
                        OLED_DrawPoint(_ucX,_ucY,1);
                }
            ucByte>>=1;//单字节位偏移
            _ucY++; //行偏移
        }
        if(_ucY - ucY0 == _ucHeight)//Y达到最大偏移量
        {
            _ucY=ucY0;
            _ucX++;//换列刷入
        }
    }
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_DrawData_by_Ln(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
    函数功能: [逐列式]向GRAM写入数据
    函数形参:
            u8 _ucX 点横坐标
            u8 _ucY 点纵坐标
            u8 _ucWidth 取模数据宽度
            u8 _ucHeight 取模数据高度
            const u8 *_pData 取模数据
            u8 _ucMode 正常0/反显1 
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_DrawData_by_Ln(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
{
    u8 ucX0 = _ucX;
    while (_ucHeight % 8) _ucHeight++;
    for(u8 i=0;i<(_ucHeight/8*_ucWidth);i++)
    {
        u8 ucByte = _pData[i];
        for(u8 j=0;j<8;j++)
        {
            if(!_ucMode)//normal
            {
                if(ucByte & 0x01)
                    OLED_DrawPoint(_ucX,_ucY,0);
                else 
                    OLED_DrawPoint(_ucX,_ucY,1);
            }
            else //abnormal
            {
                if(ucByte & 0x01)
                    OLED_DrawPoint(_ucX,_ucY,1);
                else 
                    OLED_DrawPoint(_ucX,_ucY,0);
            }
            ucByte >>= 1;
            _ucX ++;//列偏移
        }
        if(_ucX - ucX0 == _ucWidth)
        {
            _ucX = ucX0;
            _ucY ++;//换行刷入
        }
    }
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_DrawData_by_ColLn(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
    函数功能: [列行式]向GRAM写入数据
    函数形参: 
            u8 _ucX 点横坐标
            u8 _ucY 点纵坐标
            u8 _ucWidth 取模数据宽度
            u8 _ucHeight 取模数据高度
            const u8 *_pData 取模数据
            u8 _ucMode 正常0/反显1 
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_DrawData_by_ColLn(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
{
    u8 i,j;
    u8 ucY0 = _ucY;
    u8 ucX0 = _ucX;
    while(_ucHeight % 8) _ucHeight++;
    for(i=0;i<(_ucWidth*_ucHeight/8);i++)
    {
        u8 ucByte = *_pData++;
        for(j=0;j<8;j++)//Wr 1 byte
        {
            if(!_ucMode)
                (ucByte & 0x01) ? OLED_DrawPoint(_ucX,_ucY++,1):OLED_DrawPoint(_ucX,_ucY++,0); 
            else
                (ucByte & 0x01) ? OLED_DrawPoint(_ucX,_ucY++,0):OLED_DrawPoint(_ucX,_ucY++,1); 
            //更新数据
            ucByte >>= 1 ;
        }
        if(!(j % 8))
        {
            _ucX++;
            _ucY = ucY0;
        }
        if(_ucX - ucX0 == _ucWidth) 
        {
            _ucX = ucX0;
            ucY0 += 8;
            _ucY = ucY0;
        }
    }
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_DrawData_by_LnCol(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
    函数功能: [行列式]向GRAM写入数据
    函数形参:
            u8 _ucX 点横坐标
            u8 _ucY 点纵坐标
            u8 _ucWidth 取模数据宽度
            u8 _ucHeight 取模数据高度
            const u8 *_pData 取模数据
            u8 _ucMode 正常0/反显1 
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
static void OLED_DrawData_by_LnCol(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
{
    
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_GRAM_Refresh(void)  
    函数功能: 将显存(GRAM)中的数据刷入OLED屏幕
    函数形参: 无
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void OLED_GRAM_Refresh(void)
{
    for(u8 i=0;i<OLED_Height/8;i++)
    {
        OLED_SetPos(i,0);
        for(u8 j=0;j<OLED_Width;j++)
        {
            OLED_WR_Byte(OLED_GRAM[i][j],OLED_DAT);
        }
    }
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_DrawBMP(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMethod,u8 _ucMode)
    函数功能: 向 GRAM 刷入图片数据
    函数形参: u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMethod,u8 _ucMode
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void OLED_DrawBMP(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pBmpData,u8 _ucMethod,u8 _ucMode)
{
    switch(_ucMethod)
    {
        case M_Col:
        OLED_DrawData_by_Col(_ucX,_ucY,_ucWidth,_ucHeight,_pBmpData,_ucMode);
        break;
        case M_Ln:
        OLED_DrawData_by_Ln(_ucX,_ucY,_ucWidth,_ucHeight,_pBmpData,_ucMode);
        break;
        case M_ColLn:
        OLED_DrawData_by_ColLn(_ucX,_ucY,_ucWidth,_ucHeight,_pBmpData,_ucMode);
        break;
        case M_LnCol:
        OLED_DrawData_by_LnCol(_ucX,_ucY,_ucWidth,_ucHeight,_pBmpData,_ucMode);
        break;
    }
    
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_DrawSrting(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const char *_pStr,u8 _ucMode)
    函数功能: [列行式]向 GRAM 输出字符串
    函数形参: u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const char *_pStr,u8 _ucMode
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void OLED_DrawSrting(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const char *_pStr,u8 _ucMode)
{
    u8 Posi;
    u8 ucX0 = _ucX;
    while(*_pStr != '\0')
    {
        /*自动换行*/
        if( _ucX > (OLED_Width-1))//单词不显示在同一行
        //if((_ucX+_ucWidth) > (OLED_Width-1))
        {
            _ucY += _ucHeight;
            _ucX = ucX0;//同缩进换行
            //_ucX = 0;//顶格左对齐换行
        }
        ((*_pStr >= ' ') && (*_pStr <= '~')) ? (Posi = *_pStr++ - ' ') : (Posi = '?' - ' ');
        OLED_DrawData_by_ColLn(_ucX,_ucY,_ucWidth,_ucHeight,Ascii8x16[Posi],_ucMode);
        _ucX += _ucWidth;
    }
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_DrawChinese(u8 _ucX,u8 _ucY,u8 _ucSize,u8 _ucNum,u8 _ucMode)
    函数功能: OLED显示汉字
    函数形参: u8 _ucX,u8 _ucY,u8 _ucSize,u8 _ucNum,u8 _ucMode
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void OLED_DrawChinese(u8 _ucX,u8 _ucY,u8 _ucSize,u8 _ucNum,u8 _ucMode)
{
    OLED_DrawData_by_ColLn(_ucX,_ucY,_ucSize,_ucSize,Chinese16x16[_ucNum],_ucMode);
}

/*
****************************************************************************************************************
   
    函数原型: void OLED_Reset(void)
    函数功能: 清除 GRAM 里的值
    函数形参: 无
    硬件连接: 无
    返 回 值: 无
    
****************************************************************************************************************  
*/
void OLED_GRAM_Reset(void)
{
    for(u8 i=0;i<OLED_Height/8;i++)
    {
        for(u8 j=0;j<OLED_Width;j++)
        {
            OLED_GRAM[i][j] = 0x00;
        }
    }
}

/*
*********************************************************************************************************
*	函 数 名: void GUI_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c)
*	函数功能: 绘制直线
*	函数形参: (x1,y1) 起点坐标
              (x2,y2) 终点坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void GUI_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    
    if (delta_x > 0)incx = 1;        //设置单步方向
    else if (delta_x == 0)incx = 0;  //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0;  //水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    if (delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;
    
    for (t = 0; t <= distance + 1; t++) //画线输出
    {
        OLED_DrawPoint(uRow, uCol, c);  //画点
        xerr += delta_x ;
        yerr += delta_y ;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/*
*********************************************************************************************************
*	函 数 名: void GUI_Draw_AngleLine(uint16_t x, uint16_t y, float angle, u32 start, u32 len, uint16_t c)
*	函数功能: 绘制任意“角度”直线
*	函数形参: (x,y) - 原点坐标
              angle - 角度(6点钟方向为0°,角度为顺时针递增)
              start - 起始点距离原点的距离(从起始点向圆心开始绘制直线)
              len - 直线的长度
*	返 回 值: 无
*********************************************************************************************************
*/
void GUI_Draw_AngleLine(uint16_t x, uint16_t y, float angle, u32 start, u32 len, uint16_t c)
{
    int i;
    int x0, y0;
    float k = angle * (3.1415926535 / 180.0);
    for (i=start-len; i<start; i++)
    {
        x0 = cos(k) * i;
        y0 = sin(k) * i;
        OLED_DrawPoint(x + x0, y + y0, c);
    }
}

/*
*********************************************************************************************************
*	函 数 名: void GUI_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t c)
*	函数功能: 绘制圆型
*	函数形参: (x0,y0) - 圆心坐标
               r - 半径
*	返 回 值: 
*********************************************************************************************************
*/
void GUI_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t c)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       //判断下个点位置的标志
    while (a <= b)
    {
        OLED_DrawPoint(x0 + a, y0 - b, c);         //5
        OLED_DrawPoint(x0 + b, y0 - a, c);         //0
        OLED_DrawPoint(x0 + b, y0 + a, c);         //4
        OLED_DrawPoint(x0 + a, y0 + b, c);         //6
        OLED_DrawPoint(x0 - a, y0 + b, c);         //1
        OLED_DrawPoint(x0 - b, y0 + a, c);
        OLED_DrawPoint(x0 - a, y0 - b, c);         //2
        OLED_DrawPoint(x0 - b, y0 - a, c);         //7
        a++;
        
        //Bresenham画圆算法
        if (di < 0) di += 4 * a + 6;	
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/*
*********************************************************************************************************
*	函 数 名: void GUI_Draw_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c)
*	函数功能: 绘制矩形
*	函数形参: (sx,sy) - 左上角起点坐标
              (ex,ey) - 右下角终点坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void GUI_Draw_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c)
{
    GUI_Draw_Line(sx, sy, ex, sy, c);
    GUI_Draw_Line(sx, sy, sx, ey, c);
    GUI_Draw_Line(sx, ey, ex, ey, c);
    GUI_Draw_Line(ex, sy, ex, ey, c);
}

/*
*********************************************************************************************************
*	函 数 名: void GUI_Fill_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c)
*	函数功能: 填充矩形
*	函数形参: (sx,sy) - 左上角起点坐标
              (ex,ey) - 右下角终点坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void GUI_Fill_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c)
{
    uint16_t i, j;

    for (i = sx; i < ex; i++)
    {
        for (j = sy; j < ey; j++)
        {
            OLED_DrawPoint(i, j, c);	//设置光标位置
        }
    }
}
