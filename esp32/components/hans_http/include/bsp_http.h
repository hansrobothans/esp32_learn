/*
* @file         bsp_http.h 
* @brief        ESP32操作wifi
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       hans 
* @par Copyright (c):  
*               qq:304872739
*/
#ifndef BSP_HTTP_H
#define BSP_HTTP_H "BSP_HTTP_H"
/* =============
头文件包含
 =============*/
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_eth.h"
// #include "protocol_examples_common.h"

#include <esp_http_server.h>

//自写wifi库
#include "bsp_wifi.h"



#define TAG_HTTP "http_text"


httpd_handle_t start_webserver(void);


#endif//BSP_HTTP_H