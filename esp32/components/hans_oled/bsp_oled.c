#include "bsp_oled.h"

#include "string.h"
#include "stdlib.h"
#include "fonts.h"

#include "bsp_i2c.h"

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
void oled_init(void)
{
    //i2c初始化
    i2c_init();
    //oled配置
    oled_write_cmd(TURN_OFF_CMD);
    oled_write_cmd(0xAE);//关显示
    oled_write_cmd(0X20);//低列地址
    oled_write_cmd(0X10);//高列地址
    oled_write_cmd(0XB0);//
    oled_write_cmd(0XC8);
    oled_write_cmd(0X00);
    oled_write_cmd(0X10);
     //设置行显示的开始地址(0-63)  
    //40-47: (01xxxxx)  
    oled_write_cmd(0X40);
     //设置对比度  
    oled_write_cmd(0X81);
    oled_write_cmd(0XFF);//这个值越大，屏幕越亮(和上条指令一起使用)(0x00-0xff) 

    oled_write_cmd(0XA1);//0xA1: 左右反置，  0xA0: 正常显示（默认0xA0）
   //0xA6: 表示正常显示（在面板上1表示点亮，0表示不亮）  
    //0xA7: 表示逆显示（在面板上0表示点亮，1表示不亮）
    oled_write_cmd(0XA6); 

    oled_write_cmd(0XA8);//设置多路复用率（1-64） 
    oled_write_cmd(0X3F);//（0x01-0x3f）(默认为3f)
    oled_write_cmd(0XA4);
    //设置显示抵消移位映射内存计数器  
    oled_write_cmd(0XD3);
    oled_write_cmd(0X00);
    //设置显示时钟分频因子/振荡器频率 
    oled_write_cmd(0XD5);
    //低4位定义显示时钟(屏幕的刷新时间)（默认：0000）分频因子= [3:0]+1  
    //高4位定义振荡器频率（默认：1000） 
    oled_write_cmd(0XF0);
    //时钟预充电周期  
    oled_write_cmd(0XD9);
    oled_write_cmd(0X22);
    //设置COM硬件应脚配置  
    oled_write_cmd(0XDA);
    oled_write_cmd(0X12);
    oled_write_cmd(0XDB);
    oled_write_cmd(0X20);
    //电荷泵设置（初始化时必须打开，否则看不到显示）
    oled_write_cmd(0X8D);
    oled_write_cmd(0X14);
    //开显示
    oled_write_cmd(0XAF);
    //清屏
    oled_claer();
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

int oled_write_cmd(uint8_t command)
{
    //注释参考sht30之i2c教程
    int ret;
    //配置oled的寄存器
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                                 //新建操作I2C句柄
    ret = i2c_master_start(cmd);                                                  //启动I2C
    ret = i2c_master_write_byte(cmd, OLED_WRITE_ADDR |WRITE_BIT , ACK_CHECK_EN);  //发地址+写+检查ack
    ret = i2c_master_write_byte(cmd, WRITE_CMD, ACK_CHECK_EN);                    //发数据高8位+检查ack
    ret = i2c_master_write_byte(cmd,command, ACK_CHECK_EN);                       //发数据低8位+检查ack
    ret = i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_OLED_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) 
    {
        return ret;
    }
    return ret;
}

// int sht30_init(void)
// {
//     int ret;
//     //配置SHT30的寄存器
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();                                   //新建操作I2C句柄
//     i2c_master_start(cmd);                                                          //启动I2C
//     i2c_master_write_byte(cmd, SHT30_WRITE_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);    //发地址+写+检查ack
//     i2c_master_write_byte(cmd, CMD_FETCH_DATA_H, ACK_CHECK_EN);                     //发数据高8位+检查ack
//     i2c_master_write_byte(cmd, CMD_FETCH_DATA_L, ACK_CHECK_EN);                     //发数据低8位+检查ack
//     i2c_master_stop(cmd);                                                           //停止I2C
//     ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);        //I2C发送
//     i2c_cmd_link_delete(cmd);                                                       //删除I2C句柄
//     return ret;
// }


void bsp_oled_to_show(void)
{
	unsigned int cnt=0;
  oled_init();
  oled_show_str(0,0,  "I LOVE YOU", &Font_7x10, 1);
  oled_show_str(0,15, "WANG CHEN", &Font_7x10, 1);
  oled_show_str(0,30, "for ever ever", &Font_7x10, 1);
  oled_show_str(0,45, "shazi",&Font_7x10,1);
  vTaskDelay(10000 / portTICK_PERIOD_MS);
  while(1)
  {
     // cnt++;
     // oled_claer();
     // vTaskDelay(10 / portTICK_PERIOD_MS);
     // oled_all_on();
     // vTaskDelay(10 / portTICK_PERIOD_MS);
     // ESP_LOGI("OLED", "cnt = %d \r\n", cnt);
  }
}