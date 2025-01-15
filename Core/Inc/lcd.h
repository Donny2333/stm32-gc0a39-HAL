/****************************************************************************************************
//=========================================电源接线================================================//
//     LCD模块                STM32单片机
//      VCC          接          3.3V         //电源
//      GND          接          GND          //电源地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为4线制SPI总线
//     LCD模块                STM32单片机
//       SDA         接          PA7         //液晶屏SPI总线数据写信号
//=======================================液晶屏控制线接线==========================================//
//     LCD模块 					      STM32单片机
//       BLK         接          PA1          //液晶屏背光控制信号，如果不需要控制，接3.3V
//       SCL         接          PA5          //液晶屏SPI总线时钟信号
//       DC          接          PA3          //液晶屏数据/命令控制信号
//       RES         接          PA4          //液晶屏复位控制信号
//       CS          接          PA2          //液晶屏片选控制信号
//=========================================触摸屏触接线=========================================//
//模块不带触摸功能，不需要进行触摸屏接线
**************************************************************************************************/

#ifndef STM32_GC0A39_HAL_LCD_H
#define STM32_GC0A39_HAL_LCD_H

#include "main.h"

//LCD重要参数集
typedef struct {
    u16 width;             //LCD 宽度
    u16 height;            //LCD 高度
    u16 id;                //LCD ID
    u8 dir;                //横屏还是竖屏控制：0，竖屏；1，横屏。
    u16 wramcmd;           //开始写gram指令
    u16 setxcmd;           //设置x坐标指令
    u16 setycmd;           //设置y坐标指令
} _lcd_dev;

//LCD参数
extern _lcd_dev lcddev;    //管理LCD重要参数

#define USE_HORIZONTAL     0 //定义液晶屏顺时针旋转方向 	0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转

//定义LCD的尺寸
#define LCD_W 240
#define LCD_H 240

#define LCD_SPI_CS_GPIO         GPIOA
#define LCD_SPI_CS_PIN          GPIO_PIN_2

#define LCD_SPI_DC_GPIO         GPIOA
#define LCD_SPI_DC_PIN          GPIO_PIN_3

#define LCD_SPI_RES_GPIO        GPIOA
#define LCD_SPI_RES_PIN         GPIO_PIN_4

//GPIO置位（拉高）
#define LCD_CS_SET              HAL_GPIO_WritePin(LCD_SPI_CS_GPIO,LCD_SPI_CS_PIN,GPIO_PIN_SET)
#define LCD_DC_SET              HAL_GPIO_WritePin(LCD_SPI_DC_GPIO,LCD_SPI_DC_PIN,GPIO_PIN_SET)
#define LCD_RST_SET             HAL_GPIO_WritePin(LCD_SPI_RES_GPIO,LCD_SPI_RES_PIN,GPIO_PIN_SET)

//GPIO复位（拉低）
#define LCD_CS_CLR              HAL_GPIO_WritePin(LCD_SPI_CS_GPIO,LCD_SPI_CS_PIN,GPIO_PIN_RESET)
#define LCD_DC_CLR              HAL_GPIO_WritePin(LCD_SPI_DC_GPIO,LCD_SPI_DC_PIN,GPIO_PIN_RESET)
#define LCD_RST_CLR             HAL_GPIO_WritePin(LCD_SPI_RES_GPIO,LCD_SPI_RES_PIN,GPIO_PIN_RESET)

//画笔颜色
#define WHITE           0xFFFF
#define BLACK           0x0000
#define BLUE            0x001F
#define BRED            0XF81F
#define GRED            0XFFE0
#define GBLUE           0X07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define GREEN           0x07E0
#define CYAN            0x7FFF
#define YELLOW          0xFFE0
#define BROWN           0XBC40 //棕色
#define BRRED           0XFC07 //棕红色
#define GRAY            0X8430 //灰色
#define GRAY0           0xEF7D
#define GRAY1           0x8410 //灰色1      00000 000000 00000
#define GRAY2           0x4208
//GUI颜色

#define DARKBLUE        0X01CF //深蓝色
#define LIGHTBLUE       0X7D7C //浅蓝色
#define GRAYBLUE        0X5458 //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN      0X841F //浅绿色
#define LIGHTGRAY       0XEF5B //浅灰色(PANNEL)
#define LGRAY           0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE       0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12 //浅棕蓝色(选择条目的反色)

void LCD_Init(void);

void LCD_SetWindows(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd);

void LCD_Clear(u16 Color);

void LCD_direction(u8 direction);

#endif //STM32_GC0A39_HAL_LCD_H
