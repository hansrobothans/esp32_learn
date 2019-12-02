#ifndef BSP_ST7735_H
#define BSP_ST7735_H "BSP_ST7735_H"

#define CONFIG_ST7735_HOST_VSPI
// #define CONFIG_ST7735_HOST_HSPI

// #define CONFIG_USE_COLOR_RBG565

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "ascii_font.h"


// BSP_ST7735_BCKL		BLK//接模块BL引脚，背光可以采用IO控制或者PWM控制，也可以直接接到高电平常亮
// BSP_ST7735_CLK		SC//接模块CLK引脚,接裸屏SCL
// BSP_ST7735_MOSI		DI//接模块DIN/MOSI引脚，接裸屏SDA
// BSP_ST7735_DC		RS//接模块D/C引脚，接裸屏Pin7_A0
// BSP_ST7735_CS		CS//接模块CE引脚，接裸屏Pin12_CS
// BSP_ST7735_RST		RST//接模块RST引脚，接裸屏Pin6_RES


#ifdef CONFIG_ST7735_HOST_VSPI
#define BSP_ST7735_MISO -1
#define BSP_ST7735_MOSI 19//23  // SDA
#define BSP_ST7735_CLK 18//19
#define BSP_ST7735_CS 5//5

#define BSP_ST7735_DC 16//21
#define BSP_ST7735_RST 17//22


#elif CONFIG_ST7735_HOST_HSPI
#define BSP_ST7735_MISO -1
#define BSP_ST7735_MOSI 13  // SDA
#define BSP_ST7735_CLK 14
#define BSP_ST7735_CS 15

#define BSP_ST7735_DC 4
#define BSP_ST7735_RST 2
#endif

// LCD backlight contorl
#define BSP_ST7735_BCKL 15//18

#ifdef CONFIG_USE_COLOR_RBG565 // R-B-G 5-6-5
// Some ready-made 16-bit (RBG-565) color settings:
#define	COLOR_BLACK      0x0000
#define COLOR_WHITE      0xFFFF
#define	COLOR_RED        0xF800
#define	COLOR_GREEN      0x001F
#define	COLOR_BLUE       0x07E0
#define COLOR_CYAN       0x07FF
#define COLOR_MAGENTA    0xFFE0
#define COLOR_YELLOW     0xF81F
#define	COLOR_GRAY       0x8410
#define	COLOR_OLIVE      0x8011
#else // R-G-B 5-6-5
// Some ready-made 16-bit (RGB-565) color settings:
#define	COLOR_BLACK      0x0000
#define COLOR_WHITE      0xFFFF
#define	COLOR_RED        0xF800
#define	COLOR_GREEN      0x07E0
#define	COLOR_BLUE       0x001F
#define COLOR_CYAN       0x07FF
#define COLOR_MAGENTA    0xF81F
#define COLOR_YELLOW     0xFFE0
#define	COLOR_GRAY       0x8410
#define	COLOR_OLIVE      0x8400
#endif

#define ST7735_TFTWIDTH_128 128   // for 1.44 and mini
#define ST7735_TFTWIDTH_80 80     // for mini
#define ST7735_TFTHEIGHT_128 128  // for 1.44" display
#define ST7735_TFTHEIGHT_160 160  // for 1.8" and mini display

#define LCD_WIDTH 128
#define LCD_HEIGHT 128
// #define LCD_HEIGHT 160




// ST77XX commands
#define ST_CMD_DELAY 0x80  // special signifier for command lists

#define ST77XX_NOP 0x00
#define ST77XX_SWRESET 0x01
#define ST77XX_RDDID 0x04
#define ST77XX_RDDST 0x09

#define ST77XX_SLPIN 0x10
#define ST77XX_SLPOUT 0x11
#define ST77XX_PTLON 0x12
#define ST77XX_NORON 0x13

#define ST77XX_INVOFF 0x20
#define ST77XX_INVON 0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON 0x29
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_RAMWR 0x2C
#define ST77XX_RAMRD 0x2E

#define ST77XX_PTLAR 0x30
#define ST77XX_TEOFF 0x34
#define ST77XX_TEON 0x35
#define ST77XX_MADCTL 0x36
#define ST77XX_COLMOD 0x3A

#define ST77XX_MADCTL_MY 0x80
#define ST77XX_MADCTL_MX 0x40
#define ST77XX_MADCTL_MV 0x20
#define ST77XX_MADCTL_ML 0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1 0xDA
#define ST77XX_RDID2 0xDB
#define ST77XX_RDID3 0xDC
#define ST77XX_RDID4 0xDD

// ST7735 commands
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH 0x04

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

/*
 The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct {
  uint8_t cmd;
  uint8_t data[16];
  uint8_t databytes;  // No of data in data, 0xFF = end of cmds
} lcd_init_cmd_t;

extern uint8_t display_buff[LCD_WIDTH * LCD_HEIGHT * 2];


// st7355初始化
void st7735_init();
// 打开一个窗口
void st7735_set_address_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
// 在x，y画color颜色的点16 
void st7735_draw_pixel(int16_t x, int16_t y, uint16_t color);
void st7735_draw_pixel2(int16_t x, int16_t y, uint8_t colorh,uint8_t colorl);
// 矩形，填充颜色
void st7735_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void st7735_invert_color(int i);
// 全屏填充函数
void st7735_fill_screen(uint16_t color);
// 显示字符串
uint32_t st7735_draw_string(uint16_t x, uint16_t y, const char *pt, int16_t color, int16_t bg_color, uint8_t size);
// 显示字符
void st7735_draw_char(int16_t x, int16_t y, char c, int16_t color, int16_t bg_color, uint8_t size);

void st7735_image(void) ;

#endif
