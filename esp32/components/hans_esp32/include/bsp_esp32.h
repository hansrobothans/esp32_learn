#ifndef ESP32_H
#define ESP32_H "ESP32_H"

//
#include <stdio.h>
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

// 功能支持库
#include "bsp_queue.h"
#include "bsp_led_rgb.h"
#include "bsp_oled.h"
#include "bsp_sd_card.h"
#include "bsp_wifi.h"
#include "bsp_tcp.h"
#include "bsp_udp.h"


#endif//ESP32_H