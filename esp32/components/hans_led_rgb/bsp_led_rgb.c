#include "bsp_led_rgb.h"

QueueHandle_t bsp_led_rgb_xQueue;




/*
* 三色灯引脚初始化
* @param[in]   void             :无
* @retval      void             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_led_rgb_init(void)
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

/*
* 实现三色灯单色显示功能任务函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_led_rgb_set_rgb(void * pvParameters)
{
  // 将传入参数转化为正确类型，并接收
  char led_r_g_b_chr = *((char *)pvParameters);
  // 测试参数是否正常传递
  // printf("bsp_led_rgb_set_rgb%c\n",led_r_g_b_chr);
  switch (led_r_g_b_chr)
  {
    case 'r':
      //只点亮红灯
      gpio_set_level(LED_R_IO, on);
      gpio_set_level(LED_G_IO, off);
      gpio_set_level(LED_B_IO, off);
      break;
    case 'g':
      //只点亮绿灯        
      gpio_set_level(LED_R_IO, off);
      gpio_set_level(LED_G_IO, on);
      gpio_set_level(LED_B_IO, off);
      break;
    case 'b':
      //只点亮蓝灯
      gpio_set_level(LED_R_IO, off);
      gpio_set_level(LED_G_IO, off);
      gpio_set_level(LED_B_IO, on);
      break;
    default:
      //全灭
      gpio_set_level(LED_R_IO, off);
      gpio_set_level(LED_G_IO, off);
      gpio_set_level(LED_B_IO, off);
  }
}

/*
* 实现三色灯流水灯功能任务函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_led_rgb_pipeline_lamp(void * pvParameters)
{
  // 将传入参数转化为正确类型，并接收
	int delay_ms = *((int *)pvParameters);

  char led_r = 'r',
        led_g = 'g',
        led_b = 'b';

  // 测试参数是否正常传递
  // printf("2%d\n",delay_ms);
  // 循环流水灯
	while(1)
	{
    
    //只点亮红灯
    bsp_led_rgb_set_rgb(&led_r);

    // gpio_set_level(LED_R_IO, on);
    // gpio_set_level(LED_G_IO, off);
    // gpio_set_level(LED_B_IO, off);

    vTaskDelay(delay_ms / portTICK_PERIOD_MS);

    //只点亮绿灯
    bsp_led_rgb_set_rgb(&led_g);

    // gpio_set_level(LED_R_IO, off);
    // gpio_set_level(LED_G_IO, on);
    // gpio_set_level(LED_B_IO, off);

    vTaskDelay(delay_ms / portTICK_PERIOD_MS);

    //只点亮蓝灯
    bsp_led_rgb_set_rgb(&led_b);

    // gpio_set_level(LED_R_IO, off);
    // gpio_set_level(LED_G_IO, off);
    // gpio_set_level(LED_B_IO, on);

    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
    }
}


/*
* 创建三色灯流水灯功能任务函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_led_rgb_pipeline_lamp_task(void * pvParameters)
{
  int delay_ms = *((int *)pvParameters);
  // 测试参数有没有正常传递
	// printf("1%d\n",delay_ms);
  bsp_led_rgb_init(); 
  // 创建三色灯流水灯功能任务
	xTaskCreate(
      &bsp_led_rgb_pipeline_lamp,/* 任务函数 */
      "bsp_led_rgb_pipeline_lamp",/* 任务名称 */
      3000,/* 任务的堆栈大小 */
      &delay_ms,/* 任务的参数 */
      5,/* 任务的优先级 */
      NULL);/* 跟踪创建的任务的任务句柄 */
}

/*
* 从队列接收消息来控制三色灯灯功能任务函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
* @note        注释
*
*   创建发送消息的函数参考:
*     发送消息到队列来控制三色灯灯功能任务函数（流水效果）
*     void bsp_led_rgb_queue_send_set_rgb(void * pvParameters);
*     创建发送消息到队列来控制三色灯灯功能任务函数（流水效果）
*     void bsp_led_rgb_queue_send_set_rgb_task(void * pvParameters);
*/
void bsp_led_rgb_queue_receive_set_rgb(void * pvParameters)
{
  // // 将传入参数转化为正确类型，并接收
  // int delay_ms = *((int *)pvParameters);
  // 接受数据的结果
  BaseType_t xResult = 0;
  // 接收的字符
  bsp_led_message led_message_get = {0,' '};

  // // 队列句柄()在文件头定义
  // QueueHandle_t bsp_led_rgb_xQueue;
  // /* 创建队列，其大小可包含10个元素Data */
  bsp_led_rgb_xQueue = xQueueCreate(10, sizeof(bsp_led_message));

  // 判断是否创建成功
  if(bsp_led_rgb_xQueue != 0)
  {
    while(1)
    {
      // 接受数据
      xResult = xQueueReceive(bsp_led_rgb_xQueue,(void *)&led_message_get,( TickType_t ) 10);

      // 判断是否接受数据成功
      if(xResult == pdPASS)
      {
        // printf("接收到消息队列数据led_chr_get = %c\r\n", led_chr_get);
        // 将接收到的数据发送出去
        bsp_led_rgb_set_rgb(&led_message_get.data);
      }
    } 
  } 
}

/*
* 创建从队列接收消息来控制三色灯灯功能任务函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_led_rgb_queue_receive_set_rgb_task(void * pvParameters)
{
  bsp_led_rgb_init(); 
  // 创建从队列接收消息来控制三色灯灯功能任务
  xTaskCreate(
      &bsp_led_rgb_queue_receive_set_rgb,/* 任务函数 */
      "bsp_led_rgb_queue_receive_set_rgb",/* 任务名称 */
      4906,/* 任务的堆栈大小 */
      NULL,/* 任务的参数 */
      5,/* 任务的优先级 */
      NULL);/* 跟踪创建的任务的任务句柄 */
}


/*
* 发送消息到队列来控制三色灯灯功能任务函数（流水效果）
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_led_rgb_queue_send_set_rgb(void * pvParameters)
{
  // 定义流水灯延时时间
  int delay_ms = 1000;
  // 发送的字符
  bsp_led_message led_message_send = {0,' '};
  // 设置为流水灯效果
  while(1)
  {
    led_message_send.data = 'r';
    xQueueSend(bsp_led_rgb_xQueue,(void *) &led_message_send,0);
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);

    led_message_send.data = 'g';
    xQueueSend(bsp_led_rgb_xQueue,(void *) &led_message_send,0);
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);

    led_message_send.data = 'b';
    xQueueSend(bsp_led_rgb_xQueue,(void *) &led_message_send,0);
    vTaskDelay(delay_ms / portTICK_PERIOD_MS);
  }
}

/*
* 创建发送消息到队列来控制三色灯灯功能任务函数（流水效果）
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_led_rgb_queue_send_set_rgb_task(void * pvParameters)
{
  bsp_led_rgb_init(); 
  // 创建发送消息到队列来控制三色灯灯功能任务
  xTaskCreate(
      &bsp_led_rgb_queue_send_set_rgb,/* 任务函数 */
      "bsp_led_rgb_queue_send_set_rgb",/* 任务名称 */
      3000,/* 任务的堆栈大小 */
      NULL,/* 任务的参数 */
      5,/* 任务的优先级 */
      NULL);/* 跟踪创建的任务的任务句柄 */
}











