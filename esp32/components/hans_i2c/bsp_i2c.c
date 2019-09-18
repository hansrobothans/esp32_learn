#include "bsp_i2c.h"

/*
===========================
函数定义
=========================== 
*/


/** 
 * bsp_i2c 初始化
 * @param[in]   NULL
 * @retval      
 *              NULL                              
 * @par         修改日志 
 *               Ver0.0.1:
                     hans, 2019/09/08, 初始化版本\n 
 *               Ver0.0.2:
                     hans, 2019/09/08, 统一编程风格\n 
 */
void bsp_i2c_init(void)
{
    //i2c配置结构体
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;                    //I2C模式
    conf.sda_io_num = I2C_SDA_IO;                   //SDA IO映射
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;        //SDA IO模式
    conf.scl_io_num = I2C_SCL_IO;                   //SCL IO映射
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;        //SCL IO模式
    conf.master.clk_speed = 100000;                 //I2C CLK频率
    i2c_param_config(I2C_MASTER_NUM, &conf);        //设置I2C
    //注册I2C服务即使能
    i2c_driver_install(I2C_MASTER_NUM, conf.mode,0,0,0);
}
