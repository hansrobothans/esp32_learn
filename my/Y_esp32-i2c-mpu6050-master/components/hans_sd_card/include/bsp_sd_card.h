#ifndef BSP_SD_CARD_H
#define BSP_SD_CARD_H "BSP_SD_CARD_H"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

// #define PIN_NUM_MISO 2
// #define PIN_NUM_MOSI 15
// #define PIN_NUM_CS   13
// #define PIN_NUM_CLK  14

#define PIN_NUM_CS   14
#define PIN_NUM_MOSI 27
#define PIN_NUM_CLK  26
#define PIN_NUM_MISO 25



// #define PIN_NUM_CS   27
// #define PIN_NUM_MOSI 26
// #define PIN_NUM_CLK  25
// #define PIN_NUM_MISO 33

#define TAG_SD "This is SD_CARD"
// static const char *TAG = "example";

void sd_text(void);

#endif//BSP_SD_CARD_H	