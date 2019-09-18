/*
* @file         bsp_wifi.h 
* @brief        ESP32操作wifi
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       hans 
* @par Copyright (c):  
*               qq:304872739
*/
#ifndef BSP_WIFI_H
#define BSP_WIFI_H "BSP_WIFI_H"
/* =============
头文件包含
 =============*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "lwip/err.h"
#include "lwip/sys.h"


#define BSP_ESP_AP_WIFI_SSID      "HANS_ESP32"
#define BSP_ESP_AP_WIFI_PASSWORD      "yong0511"
#define BSP_ESP_AP_MAX_STA_CONN       3

#define BSP_ESP_STA_WIFI_SSID      "hans"
#define BSP_ESP_STA_WIFI_PASSWORD  "yong0511"
#define BSP_ESP_STA_MAXIMUM_RETRY  5

//开源一小步
void bsp_wifi_init_softap(void);
//官方例程
void bsp_wifi_init_sta(void);

#endif//SP_WIFI_H