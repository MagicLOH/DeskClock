#ifndef _OLED_H_
#define _OLED_H_

/*头文件引用*/
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include <stdio.h>
#include <math.h>

/*宏定义*/
#define OLED_SCLK PBout(14) 
#define OLED_MOSI PBout(13)
#define OLED_RES PBout(12)
#define OLED_DC PBout(1)
#define OLED_CS PAout(7)
#define OLED_CMD 0 //写命令模式
#define OLED_DAT 1 //写数据模式
#define OLED_Width 128
#define OLED_Height 64
#define M_Col   1
#define M_Ln    2
#define M_ColLn 3 
#define M_LnCol 4 

/*外部声明*/
extern const u8 Chinese16x16[][16*16/8];
extern const u8 Chinese24x24[][24*24/8];
extern const u8 Ascii8x16[][16*8/8];
extern const u8 Ascii6x8[][6*8/8];
extern const u8 BLE_32x32Bmp_Ln[32*32/8];
extern const u8 BLE_32x32Bmp_Col[32*32/8];
extern const u8 BLE_32x32Bmp_ColLn[128];
extern const u8 BLE_32x32Bmp_LnCol[128];
extern const u8 Shit_32x32Bmp_ColLn[32*32/8];
extern const u8 Sun_32x32Bmp_ColLn[32*32/8];
extern const u8 Temp_32x32Bmp_ColLn[32*32/8];
extern const u8 WIFI_32x32Bmp_ColLn[32*32/8];
extern const u8 Clock_32x32Bmp_ColLn[32*32/8];

/*函数声明*/
void OLED_Init(void);
void Display_CHN(u8 _ucPage,u8 _ucCol,u8 _ucSize,u8 _ucNum,u8 _ucType);
void Display_String(u8 _ucPage,u8 _ucCol,u8 _ucHeight,char *_pStr,u8 _ucType);
void OLED_GRAM_Refresh(void);
void OLED_DrawBMP(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const u8 *_pBmpData,u8 _ucMethod,u8 _ucMode);
void OLED_DrawSrting(u8 _ucX,u8 _ucY,u8 _ucWidth,u8 _ucHeight,const char *_pStr,u8 _ucMode);
void OLED_DrawChinese(u8 _ucX,u8 _ucY,u8 _ucSize,u8 _ucNum,u8 _ucMode);
void OLED_GRAM_Reset(void);
void GUI_Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
void GUI_Draw_AngleLine(uint16_t x, uint16_t y, float angle, u32 start, u32 len, uint16_t c);
void GUI_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t c);
void GUI_Draw_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c);
void GUI_Fill_Rect(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t c);

#endif
