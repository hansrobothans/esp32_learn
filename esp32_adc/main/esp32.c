#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#define ADC1_TEST_CHANNEL (4)

void adc1task(void * pvParameters)
{
    int a = *((int *)pvParameters);
    printf("%d\n", a);
    // adc_channel_t channel = *((adc_channel_t *)(arg));
    adc_channel_t channel = 4;
    // initialize ADC
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(channel,ADC_ATTEN_11db);
    while(1){
        printf("The adc1 value:%d\n",adc1_get_raw(channel));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

void app_main()
{
    int channel = 4;

    // xTaskCreate(
    //   &bsp_led_rgb_pipeline_lamp,/* 任务函数 */
    //   "bsp_led_rgb_pipeline_lamp",/* 任务名称 */
    //   3000,/* 任务的堆栈大小 */
    //   &delay_ms,/* 任务的参数 */
    //   5,/* 任务的优先级 */
    //   NULL);/* 跟踪创建的任务的任务句柄 */
    xTaskCreate(
        &adc1task,      /* 任务函数 */
        "adc1task",     /* 任务名称 */
        1024*3,         /* 任务的堆栈大小 */
        &channel,       /* 任务的参数 */
        10,             /* 任务的优先级 */
        NULL);          /* 跟踪创建的任务的任务句柄 */
}