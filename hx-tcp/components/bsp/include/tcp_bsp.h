
#ifndef __TCP_BSP_H__
#define __TCP_BSP_H__



#ifdef __cplusplus
extern "C" {
#endif


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#define TCP_SERVER_CLIENT_OPTION FALSE              //esp32作为client
//#define TCP_SERVER_CLIENT_OPTION TRUE              //esp32作为server

#define TAG                     "HX-TCP"            //打印的tag

//client
//STA模式配置信息,即要连上的路由器的账号密码
#define GATEWAY_SSID            "HANS"         //账号
#define GATEWAY_PAS             "yong0511"      //密码
#define TCP_SERVER_ADRESS       "192.168.1.100"     //作为client，要连接TCP服务器地址
#define TCP_PORT                10500              //统一的端口号，包括TCP客户端或者服务端

// FreeRTOS event group to signal when we are connected to wifi
#define WIFI_CONNECTED_BIT BIT0
extern EventGroupHandle_t tcp_event_group;

extern int  g_total_data;
extern bool g_rxtx_need_restart;


extern QueueHandle_t led_r_g_b_xQueue;


//using esp as station
void wifi_init_sta();

//create a tcp client socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_client();

//send data task
void send_data(void *pvParameters);
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


#ifdef __cplusplus
}
#endif


#endif /*#ifndef __TCP_BSP_H__*/

