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

#include "lcd.h"
#include "spi.h"

#define DMA_BUFFER_SIZE            64U

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u8 data) {
    LCD_CS_CLR;
    LCD_DC_CLR;
    SPI_WriteByte(data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @date       :2018-08-09
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u8 data) {
    LCD_CS_CLR;
    LCD_DC_SET;
    SPI_WriteByte(data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue) {
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void LCD_RESET(void) {
    LCD_RST_CLR;
    HAL_Delay(20);
    LCD_RST_SET;
    HAL_Delay(20);
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void LCD_WriteRAM_Prepare(void) {
    LCD_WR_REG(lcddev.wramcmd);
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/
void LCD_SetWindows(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd) {
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(xStar);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(xEnd);

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(yStar);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(yEnd);

    LCD_WriteRAM_Prepare();    //开始写入GRAM
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/
void LCD_Clear(u16 Color) {
    u8 i;
    u16 dmaBuffer[DMA_BUFFER_SIZE];
    u32 totalBytesToWrite;
    u32 bytesToWriteThisTime;

    LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1);

    for (i = 0U; i < DMA_BUFFER_SIZE; i++) {
        dmaBuffer[i] = __builtin_bswap16(Color);
    }

    totalBytesToWrite = (u32) lcddev.width * (u32) lcddev.height * (u32) sizeof(u16);
    bytesToWriteThisTime = DMA_BUFFER_SIZE * (u16) sizeof(u16);

    LCD_CS_CLR;
    LCD_DC_SET;
    while (totalBytesToWrite > 0UL) {
        if (totalBytesToWrite < bytesToWriteThisTime) {
            bytesToWriteThisTime = totalBytesToWrite;
        }
        totalBytesToWrite -= bytesToWriteThisTime;
        SPI_WriteData(&dmaBuffer, bytesToWriteThisTime);
    }
    LCD_CS_SET;
}

void LCD_Init(void) {
    LCD_RESET(); //LCD 复位

    //************* GC9A01初始化**********//
    LCD_WR_REG(0xEF);
    LCD_WR_REG(0xEB);
    LCD_WR_DATA(0x14);

    LCD_WR_REG(0xFE);
    LCD_WR_REG(0xEF);

    LCD_WR_REG(0xEB);
    LCD_WR_DATA(0x14);

    LCD_WR_REG(0x84);
    LCD_WR_DATA(0x40);

    LCD_WR_REG(0x85);
    LCD_WR_DATA(0xFF);

    LCD_WR_REG(0x86);
    LCD_WR_DATA(0xFF);

    LCD_WR_REG(0x87);
    LCD_WR_DATA(0xFF);

    LCD_WR_REG(0x88);
    LCD_WR_DATA(0x0A);

    LCD_WR_REG(0x89);
    LCD_WR_DATA(0x21);

    LCD_WR_REG(0x8A);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x8B);
    LCD_WR_DATA(0x80);

    LCD_WR_REG(0x8C);
    LCD_WR_DATA(0x01);

    LCD_WR_REG(0x8D);
    LCD_WR_DATA(0x01);

    LCD_WR_REG(0x8E);
    LCD_WR_DATA(0xFF);

    LCD_WR_REG(0x8F);
    LCD_WR_DATA(0xFF);


    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x20);

    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x08);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x05);


    LCD_WR_REG(0x90);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x08);

    LCD_WR_REG(0xBD);
    LCD_WR_DATA(0x06);

    LCD_WR_REG(0xBC);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xFF);
    LCD_WR_DATA(0x60);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x04);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA(0x13);
    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x13);

    LCD_WR_REG(0xC9);
    LCD_WR_DATA(0x22);

    LCD_WR_REG(0xBE);
    LCD_WR_DATA(0x11);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x0E);

    LCD_WR_REG(0xDF);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x0c);
    LCD_WR_DATA(0x02);

    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x2A);

    LCD_WR_REG(0xF1);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x72);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x6F);


    LCD_WR_REG(0xF2);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x2A);

    LCD_WR_REG(0xF3);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x72);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x6F);

    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x1B);
    LCD_WR_DATA(0x0B);

    LCD_WR_REG(0xAE);
    LCD_WR_DATA(0x77);

    LCD_WR_REG(0xCD);
    LCD_WR_DATA(0x63);


    LCD_WR_REG(0x70);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x03);

    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x34);

    LCD_WR_REG(0x62);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x71);
    LCD_WR_DATA(0xED);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x71);
    LCD_WR_DATA(0xEF);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x70);

    LCD_WR_REG(0x63);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x71);
    LCD_WR_DATA(0xF1);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x71);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x70);

    LCD_WR_REG(0x64);
    LCD_WR_DATA(0x28);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0xF1);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0xF1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x07);

    LCD_WR_REG(0x66);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCD);
    LCD_WR_DATA(0x67);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x67);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x98);

    LCD_WR_REG(0x74);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x85);
    LCD_WR_DATA(0x80);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4E);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x98);
    LCD_WR_DATA(0x3e);
    LCD_WR_DATA(0x07);

    LCD_WR_REG(0x35);
    LCD_WR_REG(0x21);

    LCD_WR_REG(0x11);
    HAL_Delay(120);

    LCD_WR_REG(0x29);
    LCD_direction(USE_HORIZONTAL);//设置LCD显示方向
    LCD_Clear(WHITE);//清全屏白色
}


/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/
void LCD_direction(u8 direction) {
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;
    lcddev.wramcmd = 0x2C;
    switch (direction) {
        case 0:
            lcddev.width = LCD_W;
            lcddev.height = LCD_H;
            LCD_WriteReg(0x36, 1 << 3);//BGR==1,MY==0,MX==0,MV==0
            break;
        case 1:
            lcddev.width = LCD_H;
            lcddev.height = LCD_W;
            LCD_WriteReg(0x36, (1 << 5) | (1 << 6) | (1 << 3));//BGR==1,MY==1,MX==0,MV==1
            break;
        case 2:
            lcddev.width = LCD_W;
            lcddev.height = LCD_H;
            LCD_WriteReg(0x36, (1 << 6) | (1 << 7) | (1 << 3));//BGR==1,MY==0,MX==0,MV==0
            break;
        case 3:
            lcddev.width = LCD_H;
            lcddev.height = LCD_W;
            LCD_WriteReg(0x36, (1 << 7) | (1 << 5) | (1 << 3));//BGR==1,MY==1,MX==0,MV==1
            break;
        default:
            break;
    }
}
