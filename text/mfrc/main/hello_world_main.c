/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


#include "rc522.h"

void tag_handler(uint8_t* serial_no) {
    for(int i = 0; i < 5; i++) {
        printf("%#x ", serial_no[i]);
    }
    
    printf("\n");
}

void app_main(void) {
    const rc522_start_args_t start_args = {
        .miso_io  = 25,
        .mosi_io  = 23,
        .sck_io   = 19,
        .sda_io   = 22,
        .callback = &tag_handler
    };

    rc522_start(start_args);
}

