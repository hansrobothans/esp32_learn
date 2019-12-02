#ifndef BSP_LED_RGB_H
#define BSP_LED_RGB_H "BSP_LED_RGB_H"

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

//引脚号
#define LED_R_IO 		23
#define LED_G_IO 		21
#define LED_B_IO 		22

// 灯开
#define LED_RGB_ON 1
// 灯灭
#define LED_RGB_OFF 0

extern QueueHandle_t bsp_led_rgb_xQueue;

typedef struct led_message
{
	int time;
	char data;
}bsp_led_message;



// 初始化三个灯的引脚
void bsp_led_rgb_init(void);

//三色流水灯
void bsp_led_rgb_pipeline_lamp(void * pvParameters);

// 使用多任务创建流水灯
void bsp_led_rgb_pipeline_lamp_task(void * pvParameters);

// 选择三色灯亮
void bsp_led_rgb_set_rgb(void * pvParameters);

// 从队列接收消息来控制三色灯灯功能任务函数
void bsp_led_rgb_queue_receive_set_rgb(void * pvParameters);

// 发送消息到队列来控制三色灯灯功能任务函数（流水效果）
void bsp_led_rgb_queue_send_set_rgb(void * pvParameters);

// 创建从队列接收消息来控制三色灯灯功能任务函数
void bsp_led_rgb_queue_receive_set_rgb_task(void * pvParameters);

// 创建发送消息到队列来控制三色灯灯功能任务函数（流水效果）
void bsp_led_rgb_queue_send_set_rgb_task(void * pvParameters);




#endif
