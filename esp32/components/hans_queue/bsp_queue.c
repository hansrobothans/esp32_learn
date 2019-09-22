#include "bsp_queue.h"
/*
* 接收到tcp发出的消息，将消息发给led
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bep_tcp_recive_send_to_led(void * pvParameters)
{
	#ifdef BSP_LED_RGB_H
    #ifdef BSP_TCP_H
    {
		// printf("\n\n\n\n\n\n\n\na\n\n\n\n\n\n\n\n");
		// 接受数据的结果
		BaseType_t xResult = 0;
		// tcp接收到的队列消息变量
		bsp_tcp_recive_message bsp_tcp_recive_message_v;
		// 发给led_rgb队列消息变量
		bsp_led_message led_message_send = {0,' '};
		while(1)
		{
		// 接受数据
		xResult = xQueueReceive(bsp_tcp_recive_xQueue,(void *)(&bsp_tcp_recive_message_v),( TickType_t ) 10 ) ;
		led_message_send.data = bsp_tcp_recive_message_v.data[0];
		// led_message_send.data = 'g';
		// 判断是否接受数据成功
		if(xResult == pdPASS)
		{
		  printf("接收到消息队列数据led_chr_get1 = %c\r\n", bsp_tcp_recive_message_v.data[0]);
		  // 将接收到的数据发送出去
		  xQueueSend(bsp_led_rgb_xQueue,(void *) &led_message_send,0);
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		} 
    }
    #endif//BSP_TCP_H
    #endif//BSP_LED_RGB_H
}


/*
* 创建发送消息到队列来控制三色灯灯功能任务函数（流水效果）
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bep_tcp_recive_send_to_led_task(void * pvParameters)
{
  // 创建tcp接受消息转发给led的功能任务
  xTaskCreate(
      &bep_tcp_recive_send_to_led,/* 任务函数 */
      "bep_tcp_recive_send_to_led",/* 任务名称 */
      3072,/* 任务的堆栈大小 */
      NULL,/* 任务的参数 */
      5,/* 任务的优先级 */
      NULL);/* 跟踪创建的任务的任务句柄 */
}
