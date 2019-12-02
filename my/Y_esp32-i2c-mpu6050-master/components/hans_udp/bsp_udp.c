/*
* @file         udp_bsp.c 
* @brief        wifi连接事件处理和socket收发数据处理
* @details      在官方源码的基础是适当修改调整，并增加注释
* @author       hx-zsj 
* @par Copyright (c):  
*               红旭无线开发团队，QQ群：671139854
* @par History:          
*               Ver0.0.1:
                     hx-zsj, 2018/08/10, 初始化版本\n 
*/

/* 
=============
头文件包含
=============
*/
#include "bsp_udp.h"

/*
===========================
全局变量定义
=========================== 
*/

struct sockaddr_in client_addr;                  //client地址
static unsigned int socklen = sizeof(client_addr);      //地址长度
int connect_socket=0;                          //连接socket
/*
===========================
函数声明
=========================== 
*/

/*
* 接收数据任务
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
void recv_data(void *pvParameters)
{
    int len = 0;            //长度
    char databuff[1024];    //缓存
    while (1)
    {
        //清空缓存
        memset(databuff, 0x00, sizeof(databuff));

        //读取接收数据
		len = recvfrom(connect_socket, databuff, sizeof(databuff), 0,
				(struct sockaddr *) &client_addr, &socklen);
        if (len > 0)
        {
            //打印接收到的数组
            ESP_LOGI(TAG_UDP, "UDP Client recvData: %s", databuff);
            //接收数据回发
            sendto(connect_socket, databuff, strlen(databuff), 0,
			            (struct sockaddr *) &client_addr, sizeof(client_addr));
        }
        else
        {
            //打印错误信息
            show_socket_error_reason("UDP Client recv_data", connect_socket);
            break;
        }
    }
    close_socket();

    vTaskDelete(NULL);
}
/*
* 建立udp client
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*               Ver0.0.12:
                    hx-zsj, 2018/08/09, 增加close socket\n 
*/
esp_err_t create_udp_client()
{

    ESP_LOGI(TAG_UDP, "will connect gateway ssid : %s port:%d",
             UDP_ADRESS, UDP_PORT);
    //新建socket
    connect_socket = socket(AF_INET, SOCK_DGRAM, 0);                         /*参数和TCP不同*/
    if (connect_socket < 0)
    {
        //打印报错信息
        show_socket_error_reason("create client", connect_socket);
        //新建失败后，关闭新建的socket，等待下次新建
        close(connect_socket);
        return ESP_FAIL;
    }
    //配置连接服务器信息
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(UDP_PORT);
    client_addr.sin_addr.s_addr = inet_addr(UDP_ADRESS);
    

    int len = 0;            //长度
    char databuff[1024] = "Hello Server,Please ack!!";    //缓存
    //测试udp server,返回发送成功的长度
	len = sendto(connect_socket, databuff, 1024, 0, (struct sockaddr *) &client_addr,
			sizeof(client_addr));
	if (len > 0) {
		ESP_LOGI(TAG_UDP, "Transfer data to %s:%u,ssucceed\n",
				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	} else {
        show_socket_error_reason("recv_data", connect_socket);
		close(connect_socket);
		return ESP_FAIL;
	}
    return ESP_OK;
}


/*
* 获取socket错误代码
* @param[in]   socket  		       :socket编号
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
int get_socket_error_code(int socket)
{
    int result;
    u32_t optlen = sizeof(int);
    int err = getsockopt(socket, SOL_SOCKET, SO_ERROR, &result, &optlen);
    if (err == -1)
    {
        //WSAGetLastError();
        ESP_LOGE(TAG_UDP, "socket error code:%d", err);
        // ESP_LOGE(TAG_UDP, "socket error code:%s", strerror(err));
        return -1;
    }
    return result;
}

/*
* 获取socket错误原因
* @param[in]   socket  		       :socket编号
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
int show_socket_error_reason(const char *str, int socket)
{
    int err = get_socket_error_code(socket);

    if (err != 0)
    {
        ESP_LOGW(TAG_UDP, "%s socket error reason %d %s", str, err, strerror(err));
    }

    return err;
}
/*
* 检查socket
* @param[in]   socket  		       :socket编号
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
int check_working_socket()
{
    int ret;

    ESP_LOGD(TAG_UDP, "check connect_socket");
    ret = get_socket_error_code(connect_socket);
    if (ret != 0)
    {
        ESP_LOGW(TAG_UDP, "connect socket error %d %s", ret, strerror(ret));
    }
    if (ret != 0)
    {
        return ret;
    }
    return ret;
}
/*
* 关闭socket
* @param[in]   socket  		       :socket编号
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
void close_socket()
{
    close(connect_socket);
}

/*
* 任务：建立UDP连接并从UDP接收数据
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/10, 初始化版本\n 
*/
static void udp_connect(void *pvParameters)
{
    //等待WIFI连接成功事件，死等
    xEventGroupWaitBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    ESP_LOGI(TAG_UDP, "start udp connected");

    //延时3S准备建立clien
    vTaskDelay(3000 / portTICK_RATE_MS);
    ESP_LOGI(TAG_UDP, "create udp Client");
    //建立client
    int socket_ret = create_udp_client();
    if (socket_ret == ESP_FAIL)
    {
        //建立失败
        ESP_LOGI(TAG_UDP, "create udp socket error,stop...");
        vTaskDelete(NULL);
    }
    else
    {
        //建立成功
        ESP_LOGI(TAG_UDP, "create udp socket succeed...");            
        //建立tcp接收数据任务
        if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, NULL))
        {
            //建立失败
            ESP_LOGI(TAG_UDP, "Recv task create fail!");
            vTaskDelete(NULL);
        }
        else
        {
            //建立成功
            ESP_LOGI(TAG_UDP, "Recv task create succeed!");
        }

    }
    vTaskDelete(NULL);
}






/*
* 测试函数
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/08, 初始化版本\n 
*/
void udp_text(void)
{
#if UDP_SERVER_CLIENT_OPTION
    //server，建立ap
    wifi_init_softap();
#else
    //client，建立sta
    bsp_wifi_init_station();
#endif
    //新建一个udp连接任务
    xTaskCreate(&udp_connect, "udp_connect", 4096, NULL, 5, NULL);
}






