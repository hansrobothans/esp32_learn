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
 * @param[in]   str   要显示的字符串
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

void bsp_oled_to_text(int x,int y,int l,int h)
{
  int i=0,j=0;
  bsp_oled_init();
  for(i=0;i<l;i++)
    for(j=0;j<h;j++)
    {
      bsp_oled_to_set_poxel(x+i,y+j);
    }
  bsp_oled_update_screen(); 
}

void bsp_oled_to_show(void)
{
  bsp_oled_init();
  // bsp_oled_show_str(0,0,  "I LOVE YOU", &Font_7x10, 1);
  // bsp_oled_show_str(0,15, "WANG CHEN", &Font_7x10, 1);
  // bsp_oled_show_str(0,30, "for ever ever", &Font_7x10, 1);
  // bsp_oled_show_str(0,45, "shazi",&Font_7x10,1);
  bsp_oled_show_char(15,15,'a',&Font_7x10, 1);
  vTaskDelay(10000 / portTICK_PERIOD_MS);
}