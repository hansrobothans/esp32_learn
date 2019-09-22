/*
* @file         bsp_oled.h 
* @brief        ESP32操作OLED-I2C
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       hans 
* @par Copyright (c):  
*               qq:304872739
*/
#ifndef BSP_OLED_H
#define BSP_OLED_H "BSP_OLED_H"

/* 
=============
头文件包含
=============
*/
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "esp_system.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "driver/i2c.h"


#include "fonts.h"
#include "bsp_i2c.h"

/*
===========================
宏定义
=========================== 
*/

//oled专门iic配置
#define I2C_OLED_MASTER_NUM             	I2C_NUM_1        /*!< I2C port number for master dev */


//OLED
#define OLED_WRITE_ADDR    0x78								//地址 
#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 64
#define WRITE_CMD      			 0X00
#define WRITE_DATA     			 0X40

#define TURN_OFF_CMD             0xAE                     //--turn off oled panel
#define SET1_LOW_COL_ADDR_CMD    0x00                     //---set low column address
#define SET2_HI_COL_ADDR_CMD     0x10                     //---set high column address
#define SET3_START_LINE_CMD      0x40                     //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
#define SET4_CONTR_REG           0x81                     //--set contrast control register
#define SET5_OUT_CURR            0xff                      // Set SEG Output Current Brightness
#define SET6_SEG_MAPPING         0xA1                     //--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
#define SET7_SCAN_DIR            0xC8                     //Set COM/Row Scan Direction   0xc0���·��� 0xc8����
#define SET8_NORMAL_DIS          0xA6                     //--set normal display
#define SET9_RATIO               0xA8                     //--set multiplex ratio(1 to 64)
#define SET10_DUTY               0x3f                     //--1/64 duty
#define SET11_DIS_OFFSET         0xD3                     //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
#define SET12_NO_OFFSET          0x00                     //-not offset
#define SET13_CLOCK_DIV          0xd5                     //--set display clock divide ratio/oscillator frequency
#define SET14_CLOCK_FC           0x80                     //--set divide ratio, Set Clock as 100 Frames/Sec
#define SET15_PRE_CHARGE         0xD9                     //--set pre-charge period
#define SET16_PER_CHARGE_15      0xF1                     //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
#define SET17_COM_PIN_CONF       0xDA                     //--set com pins hardware configuration
#define SET18_STG1               0x12                     
#define SET19_vCOMH              0xDB                     //--set vcomh
#define SET20_vCOM_D_LEVVEL     0x40                     //Set VCOM Deselect Level
#define SET21_PAGE_ADDR_MODE    0x20                     //-Set Page Addressing Mode (0x00/0x01/0x02)
#define SET22_STG2              0x02                     //
#define SET23_CHARGE_PUMP       0x8D                     //--set Charge Pump enable/disable
#define SET24_DIS_              0x14                     //--set(0x10) disable
#define SET25_ENTIRE_DIS        0xA4                     // Disable Entire Display On (0xa4/0xa5)
#define SET26_INV_DIS           0xA6                     // Disable Inverse Display On (0xa6/a7) 
#define TURN_ON_CMD             0xAF                     //--turn on oled panel

//定义字体
#define font_size Font_7x10		//定义字体大小为宽7高10
// #define font_size Font_11x18			//定义字体大小为宽11高18
// #define font_size Font_16x26			//定义字体大小为宽16高26

extern QueueHandle_t bsp_oled_xQueue;

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

// 接收oled队列发出的消息数据类型定义
typedef struct
{
	int id;
	char data[1024];
}bsp_oled_message;


//oled初始化
void bsp_oled_init(void);
// 向oled写命令
int bsp_oled_write_cmd(uint8_t command);
// 向oled写数据
int bsp_oled_write_data(uint8_t data);
// 向oled写长数据
int bsp_oled_write_long_data(uint8_t *data,uint16_t len);
// 将显存内容刷新到oled显示区
void oled_update_screen(void);
// 清屏
void bsp_oled_clear(void);
// 填屏,全显
void bsp_oled_all_on(void);
// 移动坐标
void bsp_oled_gotoXY(uint16_t x, uint16_t y);
// 向显存写入
void bsp_oled_drawpixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);
// 在x，y位置显示字符
char bsp_oled_show_char(uint16_t x, uint16_t y,char ch, FontDef_t* Font, SSD1306_COLOR_t color);
// 在x，y位置显示字符串
char bsp_oled_show_str(uint16_t x, uint16_t y,char* str, FontDef_t* Font, SSD1306_COLOR_t color);
// 在x，y位置显示一个点 
void bsp_oled_to_set_poxel(int x,int y);
//测试
// 显示字符和字符串
void bsp_oled_to_text(void);
// 显示一个矩形
void bsp_oled_to_show_rectangle(int x,int y,int l,int h);
//启动oled并显示默认提示语
void bsp_oled_welcome(void);
// 接收oled队列发出的消息
void bsp_tcp_recive_send_to_oled(void * pvParameters);
// 创建接收oled队列发出的消息功能任务函数
void bsp_tcp_recive_send_to_oled_task(void * pvParameters);

void bsp_oled_recive_send_to_oled_task(void * pvParameters);
void bsp_oled_recive_send_to_oled(void * pvParameters);


#endif