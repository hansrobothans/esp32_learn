#include "mpu6050.hpp"

#define ORIGINAL_OUTPUT			 (0)
#define ACC_FULLSCALE        	 (2)
#define GYRO_FULLSCALE			 (250)

#if ORIGINAL_OUTPUT == 0
	#if  ACC_FULLSCALE  == 2
		#define AccAxis_Sensitive (float)(16384)
	#elif ACC_FULLSCALE == 4
		#define AccAxis_Sensitive (float)(8192)
	#elif ACC_FULLSCALE == 8
		#define AccAxis_Sensitive (float)(4096)
	#elif ACC_FULLSCALE == 16
		#define AccAxis_Sensitive (float)(2048)
	#endif 
		
	#if   GYRO_FULLSCALE == 250
		#define GyroAxis_Sensitive (float)(131.0)
	#elif GYRO_FULLSCALE == 500
		#define GyroAxis_Sensitive (float)(65.5)
	#elif GYRO_FULLSCALE == 1000
		#define GyroAxis_Sensitive (float)(32.8)
	#elif GYRO_FULLSCALE == 2000
		#define GyroAxis_Sensitive (float)(16.4)
	#endif
		
#else
	#define AccAxis_Sensitive  (1)
	#define GyroAxis_Sensitive (1)
#endif

MPU6050::MPU6050(gpio_num_t scl, gpio_num_t sda, i2c_port_t port) {
    i2c = new I2C(scl, sda, port);
}

MPU6050::~MPU6050() {
    delete(i2c);
}

bool MPU6050::init() {
    if (!i2c -> slave_write(MPU6050_ADDR, PWR_MGMT_1  , 0x00))
        return false;
    if (!i2c -> slave_write(MPU6050_ADDR, SMPLRT_DIV  , 0x07))
        return false;
    if (!i2c -> slave_write(MPU6050_ADDR, CONFIG      , 0x07))
        return false;
    if (!i2c -> slave_write(MPU6050_ADDR, GYRO_CONFIG , 0x18))
        return false;
    if (!i2c -> slave_write(MPU6050_ADDR, ACCEL_CONFIG, 0x01))
        return false;
    return true;
}

float MPU6050::getAccX() {
    uint8_t r[0];
    i2c -> slave_read(MPU6050_ADDR, ACCEL_XOUT_H, r, 2);
    short accx = r[0] << 8 | r[1];
    return (float)accx / AccAxis_Sensitive;
}

float MPU6050::getAccY() {
    uint8_t r[0];
    i2c -> slave_read(MPU6050_ADDR, ACCEL_YOUT_H, r, 2);
    short accy = r[0] << 8 | r[1];
    return (float)accy / AccAxis_Sensitive;
}

float MPU6050::getAccZ() {
    uint8_t r[0];
    i2c -> slave_read(MPU6050_ADDR, ACCEL_ZOUT_H, r, 2);
    short accz = r[0] << 8 | r[1];
    return (float)accz / AccAxis_Sensitive;
}

float MPU6050::getGyroX() {
    uint8_t r[0];
    i2c -> slave_read(MPU6050_ADDR, GYRO_XOUT_H, r, 2);
    short gyrox = r[0] << 8 | r[1];
    return (float)gyrox / GyroAxis_Sensitive;
}

float MPU6050::getGyroY() {
    uint8_t r[0];
    i2c -> slave_read(MPU6050_ADDR, GYRO_YOUT_H, r, 2);
    short gyroy = r[0] << 8 | r[1];
    return (float)gyroy / GyroAxis_Sensitive;
}

float MPU6050::getGyroZ() {
    uint8_t r[0];
    i2c -> slave_read(MPU6050_ADDR, GYRO_ZOUT_H, r, 2);
    short gyroz = r[0] << 8 | r[1];
    return (float)gyroz / GyroAxis_Sensitive;
}

short MPU6050::getTemp() {
    uint8_t r[0];
    i2c -> slave_read(MPU6050_ADDR, TEMP_OUT_H, r, 2);
    return r[0] << 8 | r[1];
}


void mpu6050_task(void *pvParameters) {
    MPU6050 mpu(GPIO_NUM_32, GPIO_NUM_33, I2C_NUM_0);

    if(!mpu.init()) {
        ESP_LOGE("mpu6050", "init failed!");
        vTaskDelete(0);
    }
    ESP_LOGI("mpu6050", "init success!");

    float ax,ay,az,gx,gy,gz;
    float pitch, roll;
    float fpitch, froll;

    KALMAN pfilter(0.005);
    KALMAN rfilter(0.005);

    uint32_t lasttime = 0;
    int count = 0;

    while(1) {
        ax = -mpu.getAccX();
        ay = -mpu.getAccY();
        az = -mpu.getAccZ();
        gx = mpu.getGyroX();
        gy = mpu.getGyroY();
        gz = mpu.getGyroZ();
        pitch = atan(ax/az)*57.2958;
        roll = atan(ay/az)*57.2958;
        fpitch = pfilter.filter(pitch, gy);
        froll = rfilter.filter(roll, -gx);
        count++;
        if(esp_log_timestamp() / 1000 != lasttime) {
            lasttime = esp_log_timestamp() / 1000;
            ESP_LOGI("mpu6050", "Samples: %d", count);
            count = 0;
            ESP_LOGI("mpu6050", "Acc: ( %.3f, %.3f, %.3f)", ax, ay, az);
            ESP_LOGI("mpu6050", "Gyro: ( %.3f, %.3f, %.3f)", gx, gy, gz);
            ESP_LOGI("mpu6050", "Pitch: %.3f", pitch);
            ESP_LOGI("mpu6050", "Roll: %.3f", roll);
            ESP_LOGI("mpu6050", "FPitch: %.3f", fpitch);
            ESP_LOGI("mpu6050", "FRoll: %.3f", froll);
        }
    }

}