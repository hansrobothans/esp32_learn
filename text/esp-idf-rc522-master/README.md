# esp-idf-rc522
C library for interfacing ESP32 with MFRC522 RFID card reader

# How to use

This directory is an ESP-IDF component. Clone it (or add it as submodule) into component directory of the project.

Example:

```c
#include "rc522.c"

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
```