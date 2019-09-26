#include "bsp_oled.h"


/*
===========================
全局变量定义
=========================== 
*/
//OLED缓存128*64bit
static uint8_t g_oled_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
//OLED实时信息
static SSD1306_t oled;
//OLED是否正在显示，1显示，0等待
static bool is_show_str =0;
//oled显示文字队列
QueueHandle_t bsp_oled_xQueue;

/*
===========================
函数定义
=========================== 
*/

/** 
 * 初始化 oled
 * @param[in]   NULL
 * @retval      
 *              NULL                            
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
void bsp_oled_init(void)
{
  //i2c初始化
  bsp_i2c_init();
  //oled配置
  bsp_oled_write_cmd(TURN_OFF_CMD);
  bsp_oled_write_cmd(0xAE);//关显示
  bsp_oled_write_cmd(0X20);//低列地址
  bsp_oled_write_cmd(0X10);//高列地址
  bsp_oled_write_cmd(0XB0);//
  bsp_oled_write_cmd(0XC8);
  bsp_oled_write_cmd(0X00);
  bsp_oled_write_cmd(0X10);
   //设置行显示的开始地址(0-63)  
  //40-47: (01xxxxx)  
  bsp_oled_write_cmd(0X40);
   //设置对比度  
  bsp_oled_write_cmd(0X81);
  bsp_oled_write_cmd(0XFF);//这个值越大，屏幕越亮(和上条指令一起使用)(0x00-0xff) 

  bsp_oled_write_cmd(0XA1);//0xA1: 左右反置，  0xA0: 正常显示（默认0xA0）
 //0xA6: 表示正常显示（在面板上1表示点亮，0表示不亮）  
  //0xA7: 表示逆显示（在面板上0表示点亮，1表示不亮）
  bsp_oled_write_cmd(0XA6); 

  bsp_oled_write_cmd(0XA8);//设置多路复用率（1-64） 
  bsp_oled_write_cmd(0X3F);//（0x01-0x3f）(默认为3f)
  bsp_oled_write_cmd(0XA4);
  //设置显示抵消移位映射内存计数器  
  bsp_oled_write_cmd(0XD3);
  bsp_oled_write_cmd(0X00);
  //设置显示时钟分频因子/振荡器频率 
  bsp_oled_write_cmd(0XD5);
  //低4位定义显示时钟(屏幕的刷新时间)（默认：0000）分频因子= [3:0]+1  
  //高4位定义振荡器频率（默认：1000） 
  bsp_oled_write_cmd(0XF0);
  //时钟预充电周期  
  bsp_oled_write_cmd(0XD9);
  bsp_oled_write_cmd(0X22);
  //设置COM硬件应脚配置  
  bsp_oled_write_cmd(0XDA);
  bsp_oled_write_cmd(0X12);
  bsp_oled_write_cmd(0XDB);
  bsp_oled_write_cmd(0X20);
  //电荷泵设置（初始化时必须打开，否则看不到显示）
  bsp_oled_write_cmd(0X8D);
  bsp_oled_write_cmd(0X14);
  //开显示
  bsp_oled_write_cmd(0XAF);
  //清屏
  bsp_oled_clear();
}

/** 
 * 向oled写命令
 * @param[in]   command
 * @retval      
 *              - ESP_OK                              
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 *               Ver0.0.2:
                     hx-zsj, 2018/08/07, 统一编程风格\n 
 */

int bsp_oled_write_cmd(uint8_t command)
{
    int ret;
    //配置oled的寄存器
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                                 //新建操作I2C句柄
    ret = i2c_master_start(cmd);                                                  //启动I2C
    ret = i2c_master_write_byte(cmd, OLED_WRITE_ADDR |WRITE_BIT , ACK_CHECK_EN);  //发地址+写+检查ack
    ret = i2c_master_write_byte(cmd, WRITE_CMD, ACK_CHECK_EN);                    //发数据高8位+检查ack
    ret = i2c_master_write_byte(cmd,command, ACK_CHECK_EN);                       //发数据低8位+检查ack
    ret = i2c_master_stop(cmd);                                                   //停止I2C
    ret = i2c_master_cmd_begin(I2C_OLED_MASTER_NUM, cmd, 100 / portTICK_RATE_MS); //I2C发送
    i2c_cmd_link_delete(cmd);                                                     //删除I2C句柄
    if (ret != ESP_OK) 
    {
        return ret;
    }
    return ret;
}

/** 
 * 向oled写数据
 * @param[in]   data
 * @retval      
 *              - ESP_OK                              
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
int bsp_oled_write_data(uint8_t data)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, OLED_WRITE_ADDR | WRITE_BIT, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, WRITE_DATA, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_OLED_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) 
    {
        return ret;
    }
    return ret;
}

/** 
 * 向oled写长数据
 * @param[in]   data   要写入的数据
 * @param[in]   len     数据长度
 * @retval      
 *              - ESP_OK                              
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
int bsp_oled_write_long_data(uint8_t *data,uint16_t len)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ret = i2c_master_start(cmd);
    ret = i2c_master_write_byte(cmd, OLED_WRITE_ADDR | WRITE_BIT, ACK_CHECK_EN);
    ret = i2c_master_write_byte(cmd, WRITE_DATA, ACK_CHECK_EN);
    ret = i2c_master_write(cmd, data, len,ACK_CHECK_EN);
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_OLED_MASTER_NUM, cmd, 10000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) 
    {
        return ret;
    }
    return ret;    
}

/** 
 * 将显存内容刷新到oled显示区
 * @param[in]   NULL
 * @retval      
 *              NULL                           
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
void bsp_oled_update_screen(void)
{
    uint8_t line_index;
    for(line_index=0    ;   line_index<8   ;  line_index++)
    {
        bsp_oled_write_cmd(0xb0+line_index);
        bsp_oled_write_cmd(0x00);
        bsp_oled_write_cmd(0x10);
        
        bsp_oled_write_long_data(&g_oled_buffer[SSD1306_WIDTH * line_index],SSD1306_WIDTH);
    }
}

/** 
 * 清屏
 * @param[in]   NULL
 * @retval      
 *              NULL                            
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
void bsp_oled_clear(void)
{
    //清0缓存
    memset(g_oled_buffer,SSD1306_COLOR_BLACK,sizeof(g_oled_buffer));
    bsp_oled_update_screen();
}

/** 
 * 填屏
 * @param[in]   NULL
 * @retval      
 *              NULL                            
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
void bsp_oled_all_on(void)
{
    //置ff缓存
    memset(g_oled_buffer,0xff,sizeof(g_oled_buffer));
    bsp_oled_update_screen();
}

/** 
 * 移动坐标
 * @param[in]   x   显示区坐标 x
 * @param[in]   y   显示去坐标 y
 * @retval      
 *              其它                         
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
void bsp_oled_gotoXY(uint16_t x, uint16_t y) 
{
  oled.CurrentX = x;
  oled.CurrentY = y;
}


/** 
 * 向显存写入
 * @param[in]   x   坐标
 * @param[in]   y   坐标
 * @param[in]   color   色值0/1
 * @retval      
 *              - ESP_OK                              
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
void bsp_oled_drawpixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) 
{
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
  {
    return;
  }
  if (color == SSD1306_COLOR_WHITE) 
  {
    g_oled_buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
  } 
  else
  {
    g_oled_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
  }
}

/** 
 * 在x，y位置显示字符
 * @param[in]   x    显示坐标x 
 * @param[in]   y    显示坐标y 
 * @param[in]   ch   要显示的字符
 * @param[in]   font 显示的字形
 * @param[in]   color 颜色  1显示 0不显示
 * @retval      
 *              其它                        
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
char bsp_oled_show_char(uint16_t x,uint16_t y,char ch, FontDef_t* Font, SSD1306_COLOR_t color) 
{
  uint32_t i, b, j;
  if ( SSD1306_WIDTH <= (oled.CurrentX + Font->FontWidth) || SSD1306_HEIGHT <= (oled.CurrentY + Font->FontHeight) ) 
    {
    return 0;
  }
  if(0 == is_show_str)
    {
        bsp_oled_gotoXY(x,y);
    }

  for (i = 0; i < Font->FontHeight; i++) 
    {
    b = Font->data[(ch - 32) * Font->FontHeight + i];
    for (j = 0; j < Font->FontWidth; j++)
        {
      if ((b << j) & 0x8000) 
            {
        bsp_oled_drawpixel(oled.CurrentX + j, (oled.CurrentY + i), (SSD1306_COLOR_t) color);
      } 
            else 
            {
        bsp_oled_drawpixel(oled.CurrentX + j, (oled.CurrentY + i), (SSD1306_COLOR_t)!color);
      }
    }
  }
  oled.CurrentX += Font->FontWidth;
  if(0 == is_show_str)
    {
       bsp_oled_update_screen(); 
    }
  return ch;
}

/** 
 * 在x，y位置显示字符串 
 * @param[in]   x    显示坐标x 
 * @param[in]   y    显示坐标y 
 * @param[in]   str   要显示的字符串--------横向最多显示18个Font_7x10类型的字母（128*64像素），18*7=126
 * @param[in]   font 显示的字形
 * @param[in]   color 颜色  1显示 0不显示
 * @retval      
 *              其它                        
 * @par         修改日志 
 *               Ver0.0.1:
                     XinC_Guo, 2018/07/18, 初始化版本\n 
 */
char bsp_oled_show_str(uint16_t x,uint16_t y, char* str, FontDef_t* Font, SSD1306_COLOR_t color) 
{
  is_show_str=1;
  bsp_oled_gotoXY(x,y);
  while (*str) 
  {
    if (bsp_oled_show_char(x,y,*str, Font, color) != *str) 
      {
        is_show_str=0;
        return *str;
      }
    str++;
  }
  is_show_str=0;
  bsp_oled_update_screen();
  return *str;
}
/** 
 * 在x，y位置显示一个点 
 * @param[in]   x    显示坐标x 
 * @param[in]   y    显示坐标y 
 * @retval      
 *              如需清屏，请自行调用清屏函数清屏                       
 * @par         修改日志 
 *               Ver0.0.1:
                     hans, 2019/09/18, 初始化版本\n 
 */
void bsp_oled_to_set_poxel(int x,int y)
{
  int h_s = (y-1)/(SSD1306_HEIGHT / 8);//计算行数
  int h_p = (y-1)%(SSD1306_HEIGHT / 8);//计算行偏
  int oled_array_index = h_s * SSD1306_WIDTH + x - 1;//计算数组下标
  switch (h_p)
  {
    // 根据行偏计算将哪位置高
    case 0: g_oled_buffer[oled_array_index] = g_oled_buffer[oled_array_index] | 0x01;break;
    case 1: g_oled_buffer[oled_array_index] = g_oled_buffer[oled_array_index] | 0x02;break;
    case 2: g_oled_buffer[oled_array_index] = g_oled_buffer[oled_array_index] | 0x04;break;
    case 3: g_oled_buffer[oled_array_index] = g_oled_buffer[oled_array_index] | 0x08;break;
    case 4: g_oled_buffer[oled_array_index] = g_oled_buffer[oled_array_index] | 0x10;break;
    case 5: g_oled_buffer[oled_array_index] = g_oled_buffer[oled_array_index] | 0x20;break;
    case 6: g_oled_buffer[oled_array_index] = g_oled_buffer[oled_array_index] | 0x40;break;
    case 7: g_oled_buffer[oled_array_index] = g_oled_buffer[oled_array_index] | 0x80;break;
  }
}


/** 
 * 在x，y位置显示一个矩形 
 * @param[in]   x    坐标x 
 * @param[in]   y    坐标y 
 * @param[in]   l    矩形宽
 * @param[in]   h    矩形高
 * @retval      
 *              如需清屏，请自行调用清屏函数清屏                       
 * @par         修改日志 
 *               Ver0.0.1:
                     hans, 2019/09/18, 初始化版本\n 
 */
void bsp_oled_to_show_rectangle(int x,int y,int l,int h)
{
  int i=0,j=0;
  bsp_oled_init();
  if(x+l<127 && y+h<63)
  {
    for(i=0;i<l;i++)
      for(j=0;j<h;j++)
      {
        bsp_oled_to_set_poxel(x+i,y+j);
      }
  }
  else
  {
    bsp_oled_show_str(0,0,  "please show ", &font_size, 1);
    bsp_oled_show_str(0,10, "rectangle with: ", &font_size, 1);
    bsp_oled_show_str(0,20, "x + l < 128", &font_size, 1);
    bsp_oled_show_str(0,30, "y + h < 64", &font_size, 1);
    bsp_oled_show_str(0,40, "thank you", &font_size, 1);
    bsp_oled_show_str(0,50, "have this API", &font_size, 1);
  }

  bsp_oled_update_screen(); 
}

void bsp_oled_welcome(void)
{
  bsp_oled_init();
  bsp_oled_show_str(10,25,  "Welcome to OLED", &font_size, 1);
  vTaskDelay(10000 / portTICK_PERIOD_MS);
}


void bsp_oled_queue_str(char *oled_message_get_data)
{
  // 传入队列的数据下标
  int i = 0;
  // 存储数组的行
  int str_6_h = 0;
  // 存储数组的列
  int str_6_l = 0;

  // 中间变量
  char chr = 0;
  // 用来存储显示的字符数组
  char str_6[6][19] = {0};
  chr = oled_message_get_data[i];
  while(chr != 0)
  {
    // 高度越界
    if(str_6_h>6)
      break;
    str_6[str_6_h][str_6_l] = chr;
    str_6_l++;
    i++;
    // 长度越界
    if(str_6_l>17)
    {
      str_6[str_6_h][18] = 0;
      str_6_l = 0;
      str_6_h++;
    }
    chr = oled_message_get_data[i];
  }
  // 字符串补零
  str_6[str_6_h][str_6_l+1] = 0;
  // 开始显示
  bsp_oled_show_str(0,0,  str_6[0], &font_size, 1);
  bsp_oled_show_str(0,10, str_6[1], &font_size, 1);
  bsp_oled_show_str(0,20, str_6[2], &font_size, 1);
  bsp_oled_show_str(0,30, str_6[3], &font_size, 1);
  bsp_oled_show_str(0,40, str_6[4], &font_size, 1);
  bsp_oled_show_str(0,50, str_6[5], &font_size, 1);
  // vTaskDelay(10000 / portTICK_PERIOD_MS);
}




/*
* 接收oled队列发出的消息，x
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_tcp_recive_send_to_oled(void * pvParameters)
{
  // // 将传入参数转化为正确类型，并接收
  // int delay_ms = *((int *)pvParameters);
  int i = 0;
  // 字符串长度
  int str_len = 0;
  // 接受数据的结果
  BaseType_t xResult = 0;
  // 接收的字符
  bsp_oled_message oled_message_get;

  // // 队列句柄()在文件头定义
  // QueueHandle_t bsp_led_rgb_xQueue;
  // /* 创建队列，其大小可包含10个元素Data */
  bsp_oled_xQueue = xQueueCreate(10, sizeof(bsp_oled_message));

  // 判断是否创建成功
  if(bsp_oled_xQueue != 0)
  {
    while(1)
    {
      // printf("tcp_oled4");
      //清空缓存
      memset(oled_message_get.data, 0x00, sizeof(oled_message_get.data));
      // 接受数据
      xResult = xQueueReceive(bsp_oled_xQueue,(void *)&oled_message_get,( TickType_t ) 10);

      // 判断是否接受数据成功
      if(xResult == pdPASS)
      {
        // printf("tcp_oled5");
        str_len = strlen(oled_message_get.data);
        // printf("接收到消息队列数据oled_message_get.data = %s\r\n", oled_message_get.data);
        // 将接收到的数据发送出去
        if (str_len != 0)
        {
          bsp_oled_clear();
          // bsp_oled_show_str(0,i,oled_message_get.data, &font_size, 1);
          bsp_oled_queue_str(oled_message_get.data);
          i++;
          str_len = 0;
          if (i>63)
            i=0;
        }
        
      }
      // vTaskDelay(100 / portTICK_PERIOD_MS);
    } 
  } 
}


/*
* 创建接收oled队列发出的消息功能任务函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_tcp_recive_send_to_oled_task(void * pvParameters)
{
  // 创建接收oled队列发出的消息功能任务
  xTaskCreate(
      &bsp_tcp_recive_send_to_oled,/* 任务函数 */
      "bsp_tcp_recive_send_to_oled",/* 任务名称 */
      3000,/* 任务的堆栈大小 */
      NULL,/* 任务的参数 */
      5,/* 任务的优先级 */
      NULL);/* 跟踪创建的任务的任务句柄 */
}

/*
* oled队列发出的消息函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_oled_recive_send_to_oled(void * pvParameters)
{
  // 接收的字符
  bsp_oled_message oled_message_get={0,"12345678901234567890123456789012345678901234567890"};
  // 设置为流水灯效果
  while(1)
  {
    //清空缓存
    // memset(oled_message_get.data, 0x00, sizeof(oled_message_get.data));
    // for (int i = 1; i < 3; ++i)
    // {
    //   for(int j = 1; j < 10; ++j)
    //     oled_message_get.data[(i-1)*18+j-1] = '0' + j;
    // }
    

    xQueueSend(bsp_oled_xQueue,(void *) &oled_message_get,( TickType_t ) 10);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
/*
* 创建oled队列发出的消息功能任务函数
* @param[in]      void * pvParameters              :任务实现函数模板参数
* @retval         void                             :无
* @note        修改日志 
*               Ver0.0.1:
                    hans, 2019/09/04, 初始化版本\n 
*/
void bsp_oled_recive_send_to_oled_task(void * pvParameters)
{
  // 创建接收oled队列发出的消息功能任务
  xTaskCreate(
      &bsp_oled_recive_send_to_oled,/* 任务函数 */
      "bsp_oled_recive_send_to_oled",/* 任务名称 */
      3000,/* 任务的堆栈大小 */
      NULL,/* 任务的参数 */
      5,/* 任务的优先级 */
      NULL);/* 跟踪创建的任务的任务句柄 */
}


void bsp_oled_to_text(void)
{
  bsp_oled_init();
  bsp_oled_show_str(0,0,  "123456789012345678", &font_size, 1);
  bsp_oled_show_str(0,10, "123456789012345678", &font_size, 1);
  bsp_oled_show_str(0,20, "123456789012345678", &font_size, 1);
  bsp_oled_show_str(0,30, "123456789012345678", &font_size, 1);
  bsp_oled_show_str(0,40, "123456789012345678", &font_size, 1);
  bsp_oled_show_str(0,50, "123456789012345678", &font_size, 1);
  // bsp_oled_show_char(0,15,'a',&font_size, 1);
  // vTaskDelay(10000 / portTICK_PERIOD_MS);
}

