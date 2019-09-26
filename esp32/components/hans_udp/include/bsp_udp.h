#ifndef BSP_UDP_H
#define BSP_UDP_H

/*
=============
头文件包含
=============
*/
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

//自写wifi库
#include "bsp_wifi.h"

// #define UDP_SERVER_CLIENT_OPTION FALSE              //esp32作为client
#define UDP_SERVER_CLIENT_OPTION TRUE              //esp32作为server

#define TAG_UDP                     "hans-UDP"      //打印的tag


#define UDP_ADRESS              "192.168.123.117"   //作为client，要连接UDP服务器的地址
// #define UDP_ADRESS              "255.255.255.255"  	//如果为255.255.255.255：UDP广播
                                                    //如果为192.168.169.205：UDP单播:根据自己的server写

#define UDP_PORT                5001               //统一的端口号，包括UDP客户端或者服务端



//创建USP client
esp_err_t create_udp_client();

//收发数据
void recv_data(void *pvParameters);

//close all socket
void close_socket();

//get socket error code. return: error code
int get_socket_error_code(int socket);

//show socket error code. return: error code
int show_socket_error_reason(const char* str, int socket);

//check working socket
int check_working_socket();


void udp_text(void);



#endif /*#ifndef BSP_UDP_H*/

