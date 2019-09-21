#include "bsp_tcp.h"
/*
===========================
全局变量定义
=========================== 
*/
EventGroupHandle_t tcp_event_group;                     //wifi建立成功信号量
//socket
static int server_socket = 0;                           //服务器socket
static struct sockaddr_in server_addr;                  //server地址
static struct sockaddr_in client_addr;                  //client地址
static unsigned int socklen = sizeof(client_addr);      //地址长度
static int connect_socket = 0;                          //连接socket
bool g_rxtx_need_restart = false;                       //异常后，重新连接标记

QueueHandle_t bsp_tcp_recive_xQueue;
// 队列消息变量
bsp_tcp_recive_message bsp_tcp_recive_message_v;




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
    // // 队列消息变量
    // bsp_tcp_recive_message bsp_tcp_recive_message_v;
    // // // 队列句柄()在头文件定义
    // // QueueHandle_t bsp_tcp_recive_xQueue;
    // // /* 创建队列，其大小可包含10个元素Data */
    // bsp_tcp_recive_xQueue = xQueueCreate(10, sizeof(bsp_tcp_recive_message_v));

    int len = 0;            //长度
    char databuff[1024];    //缓存

    int i = 0;

    while (1)
    {
        //清空缓存
        memset(databuff, 0x00, sizeof(databuff));
        //读取接收数据
        len = recv(connect_socket, databuff, sizeof(databuff), 0);

        g_rxtx_need_restart = false;
        if (len > 0)
        {
            //g_total_data += len;
            ESP_LOGI(TAG_TCP, "recvData: %s", databuff);
            // bsp_tcp_recive_message_v.data = databuff;
            while(i<1024)
            {
                bsp_tcp_recive_message_v.data[i] = databuff[i];
                i++;
            }
            i=0;

            printf("%c",bsp_tcp_recive_message_v.data[0]);
            if(xQueueSend(bsp_tcp_recive_xQueue,(void *) &bsp_tcp_recive_message_v,0) != pdPASS )
                printf("向xQueue1发送数据失败\r\n");
            else
                printf("向xQueue1发送数据成功%s\r\n",bsp_tcp_recive_message_v.data);

            //接收数据回发
            send(connect_socket, databuff, strlen(databuff), 0);
        }
        else
        {
            //打印错误信息
            show_socket_error_reason("recv_data", connect_socket);
            //服务器故障，标记重连
            g_rxtx_need_restart = true;
            
	#if TCP_SERVER_CLIENT_OPTION
            //服务器接收异常，不用break后close socket,因为有其他client
            //break;
            vTaskDelete(NULL);
	#else
            //client
            break;
	#endif
        }
    }
    close_socket();
    //标记重连
    g_rxtx_need_restart = true;
    vTaskDelete(NULL);
}

/*
* 建立tcp server
* @param[in]   isCreatServer  	    :首次true，下次false
* @retval      void                 :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
esp_err_t create_tcp_server(bool isCreatServer)
{
    //首次建立server
    if (isCreatServer)
    {
        ESP_LOGI(TAG_TCP, "server socket....,port=%d", TCP_PORT);
        //新建socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0)
        {
            show_socket_error_reason("create_server", server_socket);
            //新建失败后，关闭新建的socket，等待下次新建
            close(server_socket);
            return ESP_FAIL;
        }
        //配置新建server socket参数
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(TCP_PORT);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        //bind:地址的绑定
        if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            show_socket_error_reason("bind_server", server_socket);
            //bind失败后，关闭新建的socket，等待下次新建
            close(server_socket);
            return ESP_FAIL;
        }
    }
    //listen，下次时，直接监听
    if (listen(server_socket, 5) < 0)
    {
        show_socket_error_reason("listen_server", server_socket);
        //listen失败后，关闭新建的socket，等待下次新建
        close(server_socket);
        return ESP_FAIL;
    }
    //accept，搜寻全连接队列
    connect_socket = accept(server_socket, (struct sockaddr *)&client_addr, &socklen);
    if (connect_socket < 0)
    {
        show_socket_error_reason("accept_server", connect_socket);
        //accept失败后，关闭新建的socket，等待下次新建
        close(server_socket);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG_TCP, "tcp connection established!");
    return ESP_OK;
}


/*
* 建立tcp client
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*               Ver0.0.12:
                    hx-zsj, 2018/08/09, 增加close socket\n 
*/
esp_err_t create_tcp_client()
{

    ESP_LOGI(TAG_TCP, "will connect gateway ssid : %s port:%d",
             TCP_SERVER_ADRESS, TCP_PORT);
    //新建socket
    connect_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_socket < 0)
    {
        //打印报错信息
        show_socket_error_reason("create client", connect_socket);
        //新建失败后，关闭新建的socket，等待下次新建
        close(connect_socket);
        return ESP_FAIL;
    }
    //配置连接服务器信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_PORT);
    server_addr.sin_addr.s_addr = inet_addr(TCP_SERVER_ADRESS);
    ESP_LOGI(TAG_TCP, "connectting server...");
    //连接服务器
    if (connect(connect_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        //打印报错信息
        show_socket_error_reason("client connect", connect_socket);
        ESP_LOGE(TAG_TCP, "connect failed!");
        //连接失败后，关闭之前新建的socket，等待下次新建
        close(connect_socket);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG_TCP, "connect success!");
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
        ESP_LOGE(TAG_TCP, "socket error code:%d", err);
        ESP_LOGE(TAG_TCP, "socket error code:%s", strerror(err));
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
        ESP_LOGW(TAG_TCP, "%s socket error reason %d %s", str, err, strerror(err));
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
	#if EXAMPLE_ESP_TCP_MODE_SERVER
    ESP_LOGD(TAG_TCP, "check server_socket");
    ret = get_socket_error_code(server_socket);
    if (ret != 0)
    {
        ESP_LOGW(TAG_TCP, "server socket error %d %s", ret, strerror(ret));
    }
    if (ret == ECONNRESET)
    {
        return ret;
    }
	#endif
    ESP_LOGD(TAG_TCP, "check connect_socket");
    ret = get_socket_error_code(connect_socket);
    if (ret != 0)
    {
        ESP_LOGW(TAG_TCP, "connect socket error %d %s", ret, strerror(ret));
    }
    if (ret != 0)
    {
        return ret;
    }
    return 0;
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
    close(server_socket);
}

/*
* 任务：建立TCP连接并从TCP接收数据
* @param[in]   void                :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
static void tcp_connect(void *pvParameters)
{
    // // 队列句柄()在头文件定义
    // QueueHandle_t bsp_tcp_recive_xQueue;
    // /* 创建队列，其大小可包含10个元素Data */
    bsp_tcp_recive_xQueue = xQueueCreate(10, sizeof(bsp_tcp_recive_message_v));
    while (1)
    {
        g_rxtx_need_restart = false;
        //等待WIFI连接信号量，死等
        xEventGroupWaitBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
        ESP_LOGI(TAG_TCP, "start tcp connected");

        TaskHandle_t tx_rx_task = NULL;
	#if TCP_SERVER_CLIENT_OPTION
        //延时3S准备建立server
        vTaskDelay(3000 / portTICK_RATE_MS);
        ESP_LOGI(TAG_TCP, "create tcp server");
        //建立server
        int socket_ret = create_tcp_server(true);
	#else
        //延时3S准备建立clien
        vTaskDelay(3000 / portTICK_RATE_MS);
        ESP_LOGI(TAG_TCP, "create tcp Client");
        //建立client
        int socket_ret = create_tcp_client();
	#endif
        if (socket_ret == ESP_FAIL)
        {
            //建立失败
            ESP_LOGI(TAG_TCP, "create tcp socket error,stop...");
            continue;
        }
        else
        {
            //建立成功
            ESP_LOGI(TAG_TCP, "create tcp socket succeed...");            
            //建立tcp接收数据任务
            if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task))
            {
                //建立失败
                ESP_LOGI(TAG_TCP, "Recv task create fail!");
            }
            else
            {
                //建立成功
                ESP_LOGI(TAG_TCP, "Recv task create succeed!");
            }

        }


        while (1)
        {

            vTaskDelay(3000 / portTICK_RATE_MS);

	#if TCP_SERVER_CLIENT_OPTION
            //重新建立server，流程和上面一样
            if (g_rxtx_need_restart)
            {
                ESP_LOGI(TAG_TCP, "tcp server error,some client leave,restart...");
                //建立server
                if (ESP_FAIL != create_tcp_server(false))
                {
                    if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task))
                    {
                        ESP_LOGE(TAG_TCP, "tcp server Recv task create fail!");
                    }
                    else
                    {
                        ESP_LOGI(TAG_TCP, "tcp server Recv task create succeed!");
                        //重新建立完成，清除标记
                        g_rxtx_need_restart = false;
                    }
                }
            }
	#else
            //重新建立client，流程和上面一样
            if (g_rxtx_need_restart)
            {
                vTaskDelay(3000 / portTICK_RATE_MS);
                ESP_LOGI(TAG_TCP, "reStart create tcp client...");
                //建立client
                int socket_ret = create_tcp_client();

                if (socket_ret == ESP_FAIL)
                {
                    ESP_LOGE(TAG_TCP, "reStart create tcp socket error,stop...");
                    continue;
                }
                else
                {
                    ESP_LOGI(TAG_TCP, "reStart create tcp socket succeed...");
                    //重新建立完成，清除标记
                    g_rxtx_need_restart = false;
                    //建立tcp接收数据任务
                    if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task))
                    {
                        ESP_LOGE(TAG_TCP, "reStart Recv task create fail!");
                    }
                    else
                    {
                        ESP_LOGI(TAG_TCP, "reStart Recv task create succeed!");
                    }
                }
                
                
            }
	#endif
        }
    }

    vTaskDelete(NULL);
}


/*
* 初始化tcp连接
* @param[in]   void                :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/08, 初始化版本\n 
*/
void bsp_tcp_init(void)
{
    #if TCP_SERVER_CLIENT_OPTION
        //server，建立ap
        bsp_wifi_init_softap();
    #else
        //client，建立sta
        bsp_wifi_init_station();
    #endif
        // 新建一个tcp连接任务
        xTaskCreate(&tcp_connect, "tcp_connect", 4096, NULL, 5, NULL);
}

/*初始化tcp连接任务函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
// void bsp_tcp_init_task(void * pvParameters)
// {
//   // 创建tcp接受消息转发给led的功能任务
//   xTaskCreate(
//       &bsp_tcp_init,/* 任务函数 */
//       "bsp_tcp_init",/* 任务名称 */
//       5000,/* 任务的堆栈大小 */
//       NULL,/* 任务的参数 */
//       5,/* 任务的优先级 */
//       NULL);/* 跟踪创建的任务的任务句柄 */
// }