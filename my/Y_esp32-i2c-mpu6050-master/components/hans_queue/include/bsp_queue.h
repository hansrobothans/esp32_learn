#ifndef BSP_QUEUE_H
#define BSP_QUEUE_H "BSP_QUEUE_H"

# include"bsp_esp32.h"

// 接收到tcp发出的消息，将消息发给led
void bsp_queue_tcp_recive_send_to_led(void * pvParameters);
// 接收到tcp发出的消息，将消息发给oled
void bsp_queue_tcp_recive_send_to_oled(void * pvParameters);
// 创建发送消息到队列来控制三色灯灯功能任务函数（流水效果）
void bsp_queue_tcp_recive_send_to_led_task(void * pvParameters);
// 创建接收到tcp发出的消息，将消息发给oled功能任务函数
void bsp_queue_tcp_recive_send_to_oled_task(void * pvParameters);



#endif//BSP_QUEUE_H
