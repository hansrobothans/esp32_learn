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
#define BSP_ST7735_MOSI 23  // SDA
#define BSP_ST7735_CLK 18
#define BSP_ST7735_CS 5

#define BSP_ST7735_DC 21
#define BSP_ST7735_RST 22


#elif CONFIG_ST7735_HOST_HSPI
#define BSP_ST7735_MISO -1
#define BSP_ST7735_MOSI 13  // SDA
#define BSP_ST7735_CLK 14
#define BSP_ST7735_CS 15

#define BSP_ST7735_DC 4
#define BSP_ST7735_RST 2
#endif

// LCD backlight contorl
#define BSP_ST7735_BCKL 19

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

// st7355初始化
void st7735_init();
// 矩形，填充颜色
void st7735_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void st7735_invert_color(int i);
// 全屏填充函数
void st7735_fill_screen(uint16_t color);
// 显示字符串
uint32_t st7735_draw_string(uint16_t x, uint16_t y, const char *pt, int16_t color, int16_t bg_color, uint8_t size);
// 显示字符
void st7735_draw_char(int16_t x, int16_t y, char c, int16_t color, int16_t bg_color, uint8_t size);


#endif
