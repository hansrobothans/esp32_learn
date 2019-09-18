#ifndef BSP_I2C_H
#define BSP_I2C_H "BSP_I2C_H"

#include <stdio.h>
#include "esp_system.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "driver/i2c.h"

/*
===========================
宏定义
=========================== 
*/

//I2C
#define I2C_SCL_IO       	33									//SCL->IO33
#define I2C_SDA_IO         	32              					//SDA->IO32
#define I2C_MASTER_NUM      I2C_NUM_1        					//I2C_1
#define WRITE_BIT           I2C_MASTER_WRITE 					/*!< I2C master write */
#define READ_BIT            I2C_MASTER_READ  					/*!< I2C master read */
#define ACK_CHECK_EN        0x1              					/*!< I2C master will check ack from slave主机检查从机的ACK*/
#define ACK_CHECK_DIS       0x0              					/*!< I2C master will not check ack from slave 主机不检查从机的ACK*/
#define ACK_VAL             0x0              					/*!< I2C ack value 应答*/
#define NACK_VAL            0x1              					/*!< I2C nack value 不应答*/


// i2c 初始化
void bsp_i2c_init(void);

#endif