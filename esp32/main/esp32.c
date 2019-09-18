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

# include"esp32.h"

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

    //测试oled显示屏的
    // bsp_oled_to_show();
    // bsp_oled_to_text(5,10,15,20);

    //测试sd卡
    // sd_text();
    // sd_ex();

    // 测试创建wifi
    //开源一小步
    // bsp_wifi_init_softap();
    //官方例程
    bsp_wifi_init_sta();

}
