/*
* @file         hx_tcp.c 
* @brief        ESP32搭建tcp server和client
* @details      在官方源码的基础是适当修改调整，并增加注释
* @author       hx-zsj 
* @par Copyright (c):  
*               红旭无线开发团队，QQ群：671139854
* @par History:          
*               Ver0.0.1:
                    hx-zsj, 2018/08/08, 初始化版本\n 
*               Ver0.0.2:
                    hx-zsj, 2018/08/09, 增加close socket，防止连接不上服务器多次后，就再也连不上\n 
*/

/* 
=============
头文件包含
=============
*/
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "tcp_bsp.h"

#include <stdio.h>
#include "driver/gpio.h"
#include "sdkconfig.h"

#define LED_R_IO        2
#define LED_G_IO        18
#define LED_B_IO        19

/* 这个变量保存队列句柄*/
xQueueHandle led_r_g_b_xQueue;

void led_r_g_b_init(void)
{
    //选择IO
    gpio_pad_select_gpio(LED_R_IO);
    gpio_pad_select_gpio(LED_G_IO);
    gpio_pad_select_gpio(LED_B_IO);
    //设置IO为输出
    gpio_set_direction(LED_R_IO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_G_IO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_B_IO, GPIO_MODE_OUTPUT);
}
void led_r_g_b_set(char led_r_g_b)
{
    switch (led_r_g_b)
    {
        case 'r':
            //只点亮红灯
            gpio_set_level(LED_R_IO, 1);
            gpio_set_level(LED_G_IO, 0);
            gpio_set_level(LED_B_IO, 0);
            break;
        case 'g':
            //只点亮绿灯
            gpio_set_level(LED_R_IO, 0);
            gpio_set_level(LED_G_IO, 1);
            gpio_set_level(LED_B_IO, 0);
            break;
        case 'b':
            //只点亮蓝灯
            gpio_set_level(LED_R_IO, 0);
            gpio_set_level(LED_G_IO, 0);
            gpio_set_level(LED_B_IO, 1);
            break;
        default:
            //全灭
            gpio_set_level(LED_R_IO, 0);
            gpio_set_level(LED_G_IO, 0);
            gpio_set_level(LED_B_IO, 0);

    }
}
void led_r_g_b_queue(void * pvParameters)
{
    BaseType_t xResult = 0;
    // uint8_t chr1 = 'a';
    uint8_t chr2=0;

    /* 创建队列，其大小可包含5个元素Data */
    led_r_g_b_xQueue = xQueueCreate(5, sizeof(uint8_t));

    if(led_r_g_b_xQueue != 0)
    {
        while(1)
        {
            // if(xQueueSend(led_r_g_b_xQueue,(void *) &chr1,0) != pdPASS )

            //     printf("向xQueue1发送数据失败\r\n");

            // else

            //     printf("向xQueue1发送数据成功%c\r\n",chr1);


            xResult = xQueueReceive(led_r_g_b_xQueue,(void *)&chr2,0);

            if(xResult == pdPASS)
            {
                printf("接收到消息队列数据chr2 = %c\r\n", chr2);
            }
            vTaskDelay(30 / portTICK_RATE_MS);
            led_r_g_b_set(chr2);


        } 
    } 
}




/*
===========================
函数定义
=========================== 
*/

/*
* 任务：建立TCP连接并从TCP接收数据
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
static void tcp_connect(void *pvParameters)
{
    while (1)
    {
        g_rxtx_need_restart = false;
        //等待WIFI连接信号量，死等
        xEventGroupWaitBits(tcp_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "start tcp connected");

        TaskHandle_t tx_rx_task = NULL;

        //延时3S准备建立clien
        vTaskDelay(3000 / portTICK_RATE_MS);
        ESP_LOGI(TAG, "create tcp Client");
        //建立client
        int socket_ret = create_tcp_client();

        if (socket_ret == ESP_FAIL)
        {
            //建立失败
            ESP_LOGI(TAG, "create tcp socket error,stop...");
            continue;
        }
        else
        {
            //建立成功
            ESP_LOGI(TAG, "create tcp socket succeed...");            
            //建立tcp接收数据任务
            if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task))
            {
                //建立失败
                ESP_LOGI(TAG, "Recv task create fail!");
            }
            else
            {
                //建立成功
                ESP_LOGI(TAG, "Recv task create succeed!");
            }

        }


        while (1)
        {

            vTaskDelay(3000 / portTICK_RATE_MS);

            //重新建立client，流程和上面一样
            if (g_rxtx_need_restart)
            {
                vTaskDelay(3000 / portTICK_RATE_MS);
                ESP_LOGI(TAG, "reStart create tcp client...");
                //建立client
                int socket_ret = create_tcp_client();

                if (socket_ret == ESP_FAIL)
                {
                    ESP_LOGE(TAG, "reStart create tcp socket error,stop...");
                    continue;
                }
                else
                {
                    ESP_LOGI(TAG, "reStart create tcp socket succeed...");
                    //重新建立完成，清除标记
                    g_rxtx_need_restart = false;
                    //建立tcp接收数据任务
                    if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task))
                    {
                        ESP_LOGE(TAG, "reStart Recv task create fail!");
                    }
                    else
                    {
                        ESP_LOGI(TAG, "reStart Recv task create succeed!");
                    }
                }
                
                
            }
        }
    }

    vTaskDelete(NULL);
}


/*
* 主函数
* @param[in]   void  		       :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/08, 初始化版本\n 
*/
void app_main(void)
{
    //初始化flash
    //确认flash没有问题
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //初始化led
    led_r_g_b_init();

    //client，建立sta
    wifi_init_sta();
    //新建一个tcp连接任务
    xTaskCreate(&tcp_connect, "tcp_connect", 4096, NULL, 5, NULL);

    xTaskCreate(
      &led_r_g_b_queue,/* 任务函数 */
      "led_r_g_b_queue",/* 任务名称 */
      3000,/* 任务的堆栈大小 */
      NULL,/* 任务的参数 */
      4,/* 任务的优先级 */
      NULL);/* 跟踪创建的任务的任务句柄 */
}
