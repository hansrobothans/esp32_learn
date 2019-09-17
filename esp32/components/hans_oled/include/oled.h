/*
* @file         oled.h 
* @brief        ESP32操作OLED-I2C
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       红旭团队 
* @par Copyright (c):  
*               红旭无线开发团队，QQ群：671139854
*/
#ifndef OLED_H
#define OLED_H

/* 
=============
头文件包含
=============
*/
#include <stdio.h>
#include "esp_system.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "driver/i2c.h"
#include "fonts.h"

//显示1，擦除0
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;

typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;


void oled_claer(void);
void oled_all_on(void);
void oled_set_pos(uint8_t x,uint8_t y);
int oled_write_data(uint8_t data);
void clean_oled_buff(void);
void oled_update_screen(void);
int oled_write_lang_data(uint8_t *data,uint16_t len);
void oled_drawpixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);
void oled_gotoXY(uint16_t x, uint16_t y) ;
char oled_show_char(uint16_t x, uint16_t y,char ch, FontDef_t* Font, SSD1306_COLOR_t color)  ;
char oled_show_str(uint16_t x, uint16_t y,char* str, FontDef_t* Font, SSD1306_COLOR_t color) ;

#endif

