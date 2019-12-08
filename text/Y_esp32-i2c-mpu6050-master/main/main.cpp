#include "mpu6050.hpp"


extern "C" void app_main()
{
    xTaskCreatePinnedToCore(&mpu6050_task,"mpu6050_task",2048,NULL,5,NULL,0);
}