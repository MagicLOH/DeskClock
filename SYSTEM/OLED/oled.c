#include "oled.h"

/*��������*/
u8 OLED_GRAM [OLED_Height/8][OLED_Width];
static void OLED_WR_Byte(u8 ,u8 );
static void OLED_SetPos(u8 _ucPage,u8 _ucCol);
static void OLED_ClearScreen(void);
static void OLED_DisplayDAT(u8 _ucPage,u8 _ucCol,u8 _ucWidth,u8 _ucHeight,const u8 *_pData);
/*
****************************************************************************************************************
   
    ����ԭ��: void OLED_Initial(void)
    ��������: OLED ��ʾ����ʼ��
    �����β�: ��
    Ӳ������: 
             D0  -- PB14 -- SPIʱ���� -- OLED_SCLK -- ͨ���������ģʽ
             D1  -- PB13 -- SPI������ -- OLED_MOSI -- ͨ���������ģʽ
             RES#-- PB12 -- ��λ��    -- OLED_RES -- ͨ���������ģʽ
             DC# -- PB1  -- DATA/CMMAND -- OLED_DC --ͨ���������ģʽ
             CS# -- PA7  -- SPIƬѡ�ź���(��������) -- OLED_CS --ͨ���������ģʽ
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void OLED_Init(void)
{
    /*GPIO�������*/
    RCC->APB2ENR |= 0x3<<2;  //GPIOA/B�˿�ʱ��ʹ��
    //PB1/12/13/14,PA7����Ϊͨ���������
    GPIOA->CRL &= 0x0FFFFFFF;
    GPIOA->CRL |= 0X30000000;// CS
    GPIOB->CRL &= 0xFFFFFF0F;
    GPIOB->CRL |= 0x00000030;// DC
    GPIOB->CRH &= 0xF000FFFF;
    GPIOB->CRH |= 0x03330000;// D0,D1,RES
    //�������ߵ�ƽ
    GPIOA->ODR |= 1<<7;
    GPIOB->ODR |= 1<<1;
    GPIOB->ODR |= 0x7<<12;
    /*OLED�������*/
    //OLEDӲ����λ
    OLED_RES = 0;//��ʼ
    delay_ms(100);
    OLED_RES = 1;//����
    delay_ms(100);

    //��ʼ������
    OLED_WR_Byte(0xAE, OLED_CMD);//--�ر���ʾ
    OLED_WR_Byte(0x00, OLED_CMD);//--set low column address
    OLED_WR_Byte(0x10, OLED_CMD);//--set high column address
    OLED_WR_Byte(0x40, OLED_CMD);//--set start line address
    OLED_WR_Byte(0x81, OLED_CMD);//--������Ļ�ԱȶȼĴ����Ĵ���
    OLED_WR_Byte(0xCF, OLED_CMD);//--���öԱȶ�[0x00~0xFF]
    OLED_WR_Byte(0xA1, OLED_CMD);//--Set SEG/Column Mapping 0xa0 ���ҷ��� 0xa1 ����
    OLED_WR_Byte(0xC8, OLED_CMD);//--Set COM/Row Scan Direction 0xc0 ���·��� 0xc8 ����
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
    OLED_WR_Byte(0x20, OLED_CMD);//--����Ѱַ��ʽ (0x00/0x01/0x02)
    OLED_WR_Byte(0x02, OLED_CMD);//--ҳ��Ѱַģʽ(ҳ���е�ַ����)
    OLED_WR_Byte(0x8D, OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14, OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4, OLED_CMD);//--Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6, OLED_CMD);//--������ʾģʽ(0Xa6 ����ģʽ��Ϊ 1/0xa7 ����ģʽ��Ϊ 0)
    OLED_WR_Byte(0xAF, OLED_CMD);//--������ʾ

    OLED_ClearScreen();
}

/*
****************************************************************************************************************
   
    ����ԭ��: static void OLED_WR_Byte(u8 _ucTxd,u8 _ucMode)
    ��������: ����SPIͨ�Ŷ�OLEDд������
    �����β�: u8 _ucTxd ��д�������,u8 _ucMode д������(0)/����(1)
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
static void OLED_WR_Byte(u8 _ucTxd,u8 _ucMode)
{
    OLED_CS = 0;      //����Ƭѡ
    OLED_DC = _ucMode;//д�����д����
    OLED_SCLK = 0;    //���õ�ƽ
    for(u8 i=0;i<8;i++)//д��һ���ֽ�
    {
        OLED_SCLK = 0;//�ȴ�������
        if(_ucTxd & 0x80)
            OLED_MOSI = 1;
        else
            OLED_MOSI = 0;
        OLED_SCLK = 1;//����ߵ�ƽ������
        _ucTxd<<=1;   //�Ƹ���һλ
    }
    OLED_SCLK = 0;//���õ�ƽ
    OLED_CS = 1;  //�ر�Ƭѡ
}

/*
****************************************************************************************************************
   
    ����ԭ��: static void OLED_SetPos(u8 _ucPage,u8 _ucCol,u8 _ucMode)
    ��������: ����д��������ʼλ��
    �����β�: u8 _ucPage,u8 _ucCol
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
static void OLED_SetPos(u8 _ucPage,u8 _ucCol)
{
    //xx & xx��ʾȡ����nλ,���ò�������
    OLED_WR_Byte(0xB0 + (_ucPage & 0x07),OLED_CMD);
    OLED_WR_Byte(0x00 + (_ucCol & 0x0F),OLED_CMD);      //����λSet the lower start column address
    OLED_WR_Byte(0x10 + ((_ucCol & 0xF0)>>4),OLED_CMD); //����λSet the upper start column address
}

/*
****************************************************************************************************************
   
    ����ԭ��: static void OLED_ClearScreen(void)
    ��������: OLED��ʾ������
    �����β�: ��
    Ӳ������: ��
    �� �� ֵ: ��
    
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
   
    ����ԭ��: static void OLED_DisplayDAT(u8 _ucPage,u8 _ucCol,u8 _ucWidth,u8 _ucHeight,const u8 *_pData)
    ��������: ����Ļ����ʾ����
    �����β�: u8 _ucPage,u8 _ucCol,u8 _ucWidth,u8 _ucHeight,const u8 *_pData
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
static void OLED_DisplayDAT(u8 _ucPage,u8 _ucCol,u8 _ucWidth,u8 _ucHeight,const u8 *_pData)
{
    while(_ucHeight % 8) _ucHeight++;//��֤����һ��
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
   
    ����ԭ��: void Display_CHN(u8 _ucPage,u8 _ucCol,u8 _ucSize,u8 _ucNum)
    ��������: ��ʾ����
    �����β�: u8 _ucPage,u8 _ucCol,u8 _ucSize,u8 _ucNum ���ֿ��еı��
    Ӳ������: ��
    �� �� ֵ: ��
    
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
   
    ����ԭ��: void Display_String(u8 _ucPage,u8 _ucCol,u8 _ucHeight,char *_pStr,u8 _ucType)
    ��������: ��ʾ�ַ���
    �����β�: 
            u8 _ucPage,
            u8 _ucCol,
            u8 _ucHeight,
            char *_pStr,
            u8 _ucType
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void Display_String(u8 _ucPage,u8 _ucCol,u8 _ucHeight,char *_pStr,u8 _ucType)
{
    while(*_pStr != '\0')
    {
        /*�Զ�����*/
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
        
        _ucCol += _ucHeight/2;//ƫ���е�ַ
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
****************************************************************************************************************
   
    ����ԭ��: void OLED_DrawPoint(u8 _ucX,u8 _ucY,u8 _ucState)
    ��������: �򻺴���GRAM����
    �����β�: u8 _ucX,u8 _ucY,u8 _ucState д��״̬ ��1/��0
    Ӳ������: ��
    �� �� ֵ: ��
    
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
   
    ����ԭ��: void OLED_DrawData_by_Col(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
    ��������: [����ʽʽ]��GRAMд������
    �����β�:
            u8 _ucX �������
            u8 _ucY ��������
            u8 _ucWidth ȡģ���ݿ��
            u8 _ucHeight ȡģ���ݸ߶�
            const u8 *_pData ȡģ����
            u8 _ucMode ����0/����1 
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
static void OLED_DrawData_by_Col(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
{
    u8 ucY0 = _ucY;//��¼Y����ʼλ��
    while(_ucHeight % 8) _ucHeight++;//����������
    for(u8 i=0;i<(_ucWidth*_ucHeight/8);i++)
    {
        u8 ucByte = _pData[i];//ȡ 1 byte����
        for(u8 j=0;j<8;j++)//ˢ�� 1 byte
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
            ucByte>>=1;//���ֽ�λƫ��
            _ucY++; //��ƫ��
        }
        if(_ucY - ucY0 == _ucHeight)//Y�ﵽ���ƫ����
        {
            _ucY=ucY0;
            _ucX++;//����ˢ��
        }
    }
}

/*
****************************************************************************************************************
   
    ����ԭ��: void OLED_DrawData_by_Ln(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
    ��������: [����ʽ]��GRAMд������
    �����β�:
            u8 _ucX �������
            u8 _ucY ��������
            u8 _ucWidth ȡģ���ݿ��
            u8 _ucHeight ȡģ���ݸ߶�
            const u8 *_pData ȡģ����
            u8 _ucMode ����0/����1 
    Ӳ������: ��
    �� �� ֵ: ��
    
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
            _ucX ++;//��ƫ��
        }
        if(_ucX - ucX0 == _ucWidth)
        {
            _ucX = ucX0;
            _ucY ++;//����ˢ��
        }
    }
}

/*
****************************************************************************************************************
   
    ����ԭ��: void OLED_DrawData_by_ColLn(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
    ��������: [����ʽ]��GRAMд������
    �����β�: 
            u8 _ucX �������
            u8 _ucY ��������
            u8 _ucWidth ȡģ���ݿ��
            u8 _ucHeight ȡģ���ݸ߶�
            const u8 *_pData ȡģ����
            u8 _ucMode ����0/����1 
    Ӳ������: ��
    �� �� ֵ: ��
    
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
            //��������
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
   
    ����ԭ��: void OLED_DrawData_by_LnCol(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
    ��������: [����ʽ]��GRAMд������
    �����β�:
            u8 _ucX �������
            u8 _ucY ��������
            u8 _ucWidth ȡģ���ݿ��
            u8 _ucHeight ȡģ���ݸ߶�
            const u8 *_pData ȡģ����
            u8 _ucMode ����0/����1 
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
static void OLED_DrawData_by_LnCol(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMode)
{
    
}

/*
****************************************************************************************************************
   
    ����ԭ��: void OLED_GRAM_Refresh(void)  
    ��������: ���Դ�(GRAM)�е�����ˢ��OLED��Ļ
    �����β�: ��
    Ӳ������: ��
    �� �� ֵ: ��
    
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
   
    ����ԭ��: void OLED_DrawBMP(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMethod,u8 _ucMode)
    ��������: �� GRAM ˢ��ͼƬ����
    �����β�: u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pData,u8 _ucMethod,u8 _ucMode
    Ӳ������: ��
    �� �� ֵ: ��
    
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
   
    ����ԭ��: void OLED_DrawSrting(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const char *_pStr,u8 _ucMode)
    ��������: [����ʽ]�� GRAM ����ַ���
    �����β�: u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const char *_pStr,u8 _ucMode
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void OLED_DrawSrting(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const char *_pStr,u8 _ucMode)
{
    u8 Posi;
    u8 ucX0 = _ucX;
    while(*_pStr != '\0')
    {
        /*�Զ�����*/
        if( _ucX > (OLED_Width-1))//���ʲ���ʾ��ͬһ��
        //if((_ucX+_ucWidth) > (OLED_Width-1))
        {
            _ucY += _ucHeight;
            _ucX = ucX0;//ͬ��������
            //_ucX = 0;//��������뻻��
        }
        ((*_pStr >= ' ') && (*_pStr <= '~')) ? (Posi = *_pStr++ - ' ') : (Posi = '?' - ' ');
        OLED_DrawData_by_ColLn(_ucX,_ucY,_ucWidth,_ucHeight,Ascii8x16[Posi],_ucMode);
        _ucX += _ucWidth;
    }
}

/*
****************************************************************************************************************
   
    ����ԭ��: void OLED_DrawChinese(u8 _ucX,u8 _ucY,u8 _ucSize,u8 _ucNum,u8 _ucMode)
    ��������: OLED��ʾ����
    �����β�: u8 _ucX,u8 _ucY,u8 _ucSize,u8 _ucNum,u8 _ucMode
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void OLED_DrawChinese(u8 _ucX,u8 _ucY,u8 _ucSize,u8 _ucNum,u8 _ucMode)
{
    OLED_DrawData_by_ColLn(_ucX,_ucY,_ucSize,_ucSize,Chinese16x16[_ucNum],_ucMode);
}

/*
****************************************************************************************************************
   
    ����ԭ��: void OLED_Reset(void)
    ��������: ��� GRAM ���ֵ
    �����β�: ��
    Ӳ������: ��
    �� �� ֵ: ��
    
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
*	�� �� ��: void GUI_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c)
*	��������: ����ֱ��
*	�����β�: (x1,y1) �������
              (x2,y2) �յ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GUI_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //������������
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    
    if (delta_x > 0)incx = 1;        //���õ�������
    else if (delta_x == 0)incx = 0;  //��ֱ��
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0;  //ˮƽ��
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    if (delta_x > delta_y)distance = delta_x; //ѡȡ��������������
    else distance = delta_y;
    
    for (t = 0; t <= distance + 1; t++) //�������
    {
        OLED_DrawPoint(uRow, uCol, c);  //����
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
*	�� �� ��: void GUI_Draw_AngleLine(uint16_t x, uint16_t y, float angle, u32 start, u32 len, uint16_t c)
*	��������: �������⡰�Ƕȡ�ֱ��
*	�����β�: (x,y) - ԭ������
              angle - �Ƕ�(6���ӷ���Ϊ0��,�Ƕ�Ϊ˳ʱ�����)
              start - ��ʼ�����ԭ��ľ���(����ʼ����Բ�Ŀ�ʼ����ֱ��)
              len - ֱ�ߵĳ���
*	�� �� ֵ: ��
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
*	�� �� ��: void GUI_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t c)
*	��������: ����Բ��
*	�����β�: (x0,y0) - Բ������
               r - �뾶
*	�� �� ֵ: 
*********************************************************************************************************
*/
void GUI_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t c)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       //�ж��¸���λ�õı�־
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
        
        //Bresenham��Բ�㷨
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
*	�� �� ��: void GUI_Draw_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c)
*	��������: ���ƾ���
*	�����β�: (sx,sy) - ���Ͻ��������
              (ex,ey) - ���½��յ�����
*	�� �� ֵ: ��
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
*	�� �� ��: void GUI_Fill_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c)
*	��������: ������
*	�����β�: (sx,sy) - ���Ͻ��������
              (ex,ey) - ���½��յ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GUI_Fill_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c)
{
    uint16_t i, j;

    for (i = sx; i < ex; i++)
    {
        for (j = sy; j < ey; j++)
        {
            OLED_DrawPoint(i, j, c);	//���ù��λ��
        }
    }
}
