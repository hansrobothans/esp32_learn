/*
* @file         bsp_tcp.h 
* @brief        ESP32操作tcp
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       hans 
* @par Copyright (c):  
*               qq:304872739
*/
#ifndef BSP_TCP_H
#define BSP_TCP_H "BSP_TCP_H"
/* =============
头文件包含
 =============*/
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
//自写wifi库
#include "bsp_wifi.h"


//更改此处，选择是作为 服务器端还是客户端
#define TCP_SERVER_CLIENT_OPTION FALSE              //esp32作为client
//#define TCP_SERVER_CLIENT_OPTION TRUE              //esp32作为server

//日志消息
#define TAG_TCP                     "HANS-TCP"            //打印的tag

//client
#define TCP_SERVER_ADRESS       "192.168.123.117"     //作为client，要连接TCP服务器地址
#define TCP_PORT                5000              //统一的端口号，包括TCP客户端或者服务端


extern int  g_total_data;
extern bool g_rxtx_need_restart;


typedef struct tcp_recive_message
{
	//预留
	char * ip;
	char * port;

	char  data[1024];
}bsp_tcp_recive_message;

extern QueueHandle_t bsp_tcp_recive_xQueue;


//create a tcp server socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_server(bool isCreatServer);
//create a tcp client socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_client();

// //send data task
// void send_data(void *pvParameters);
//receive data task
void recv_data(void *pvParameters);

//close all socket
void close_socket();

//get socket error code. return: error code
int get_socket_error_code(int socket);

//show socket error code. return: error code
int show_socket_error_reason(const char* str, int socket);

//check working socket
int check_working_socket();


//建立tcp连接，主从看宏（TCP_SERVER_CLIENT_OPTION）
void bsp_tcp_init(void);

// void bsp_tcp_init_task(void * pvParameters);

#endif//BSP_TCP_H