/*
* @file         esp.c 
* @brief        用户应用程序入口
* @details      用户应用程序的入口文件,用户所有要实现的功能逻辑均是从该文件开始或者处理
* @author       hans
* @par Copyright (c):  
*               个人开发库，QQ：304872739
* @par History:          
*               Ver0.0.1:
                     hans, 2019/09/02, 初始化版本\n 
*/

# include"bsp_esp32.h"

void app_main()
{
    // 初始化flash
    // 确认flash没有问题
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // 测试流水灯的
    // int delay_ms = 1000;
    // bsp_led_rgb_pipeline_lamp_task(&delay_ms);
    
    // 测试队列方法控制消息
    // 创建从队列接收消息来控制三色灯灯功能任务函数
    // bsp_led_rgb_queue_receive_set_rgb_task(NULL);

    // 创建发送消息到队列来控制三色灯灯功能任务函数（流水效果）
    // bsp_led_rgb_queue_send_set_rgb_task(NULL);

    //测试oled显示屏的
    // bsp_oled_to_text();
    // bsp_oled_to_show_rectangle(50,50,50,20);
    // bsp_oled_welcome();
    // bsp_oled_init();
    // bsp_tcp_recive_send_to_oled_task(NULL);
    // bsp_oled_recive_send_to_oled_task(NULL);
    // bsp_oled_queue_str(NULL);




    //测试sd卡
    // sd_text();

    // 测试创建wifi的ap
    //开源一小步
    // bsp_wifi_init_softap();
    // bsp_wifi_init_char_station_softap('a');

    //测试创建wifi的sta
    // bsp_wifi_init_station();
    // bsp_wifi_init_char_station_softap('s');

    //测试tcp连接
    // bsp_tcp_init();

    //接收到tcp发出的消息，将消息发给led
    // bsp_queue_tcp_recive_send_to_led_task(NULL);

    // 创建接收到tcp发出的消息，将消息发给oled功能任务函数
    // bsp_queue_tcp_recive_send_to_oled_task(NULL);

    // udp_text();
}   
