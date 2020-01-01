#ifndef BSP_SPI_H
#define BSP_SPI_H "BSP_SPI_H"

#include "driver/spi_master.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// #include <stdint.h>
// #include "sdkconfig.h"
// #include "esp_system.h"


#define CONFIG_ST7735_HOST_VSPI
// #define CONFIG_ST7735_HOST_HSPI


// BSP_ST7735_BCKL		BLK//接模块BL引脚，背光可以采用IO控制或者PWM控制，也可以直接接到高电平常亮
// BSP_ST7735_CLK		SC//接模块CLK引脚,接裸屏SCL
// BSP_ST7735_MOSI		DI//接模块DIN/MOSI引脚，接裸屏SDA
// BSP_ST7735_DC		RS//接模块D/C引脚，接裸屏Pin7_A0
// BSP_ST7735_CS		CS//接模块CE引脚，接裸屏Pin12_CS
// BSP_ST7735_RST		RST//接模块RST引脚，接裸屏Pin6_RES


#ifdef CONFIG_ST7735_HOST_VSPI
//测试能用
#define BSP_MISO 17
#define BSP_MOSI 23
#define BSP_CLK 22

#elif CONFIG_ST7735_HOST_HSPI
#define BSP_MISO 17
#define BSP_MOSI 23
#define BSP_CLK 22

#endif

bool bsp_spi_init(int miso_io, int mosi_io, int sck_io,int max_transfer_sz);

#endif