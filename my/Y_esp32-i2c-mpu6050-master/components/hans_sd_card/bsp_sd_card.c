# include"bsp_sd_card.h"

sdmmc_card_t* bsp_sd_card_init(void)
{
    ESP_LOGI(TAG_SD, "Initializing SD card");
    ESP_LOGI(TAG_SD, "Using SPI peripheral");

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = PIN_NUM_MISO;
    slot_config.gpio_mosi = PIN_NUM_MOSI;
    slot_config.gpio_sck  = PIN_NUM_CLK;
    slot_config.gpio_cs   = PIN_NUM_CS;
    // This initializes the slot without card detect (CD) and write protect (WP) signals.初始化插槽没有卡检测（CD）和写保护（WP）信号。
    // Modify修改 slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.信号
    //USE_SPI_MODE

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.挂载失败时会进行格式化分区
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.一体化便利的函数
    // Please check its source code and implement error recovery 请检查其源代码并实现错误恢复。
    // when developing production applications.在开发生产应用程序时
    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG_SD, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG_SD, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return NULL;
    }

    // Card has been initialized, print its properties属性
    sdmmc_card_print_info(stdout, card);

    return card;
}

void bsp_sd_card_open_to_read(char *path,char *mobel)
{
    // Check if destination file exists before renaming
    struct stat st;
    if (stat(path, &st) == 0) {
        printf("The sd exists %s\n",path);
    }
    // printf("2\n");
    // Open file for reading
    ESP_LOGI(TAG_SD, "Reading file");
    FILE* f = fopen(path, mobel);
    if (f == NULL) {
        ESP_LOGE(TAG_SD, "Failed to open file for reading");
        return;
    }
    char line[128];
    while(fgets(line, sizeof(line), f))
    {
        // strip newline
        char* pos = strchr(line, '\n');
        if (pos) {
            *pos = '\0';
        }
        ESP_LOGI(TAG_SD, "Read from file: '%s'", line);
    }
    fclose(f);
}


void sd_text(void)
{
	
    // printf("1\n");
    // sdmmc_card_t* card = bsp_sd_card_init();
    bsp_sd_card_init();

    // Use POSIX and C standard标准 library functions to work with files.  
    char* path="/sdcard/st_xhr.c";
    bsp_sd_card_open_to_read(path,"rb");

    // All done, unmount partition and disable SDMMC or SPI peripheral
    esp_vfs_fat_sdmmc_unmount();
    ESP_LOGI(TAG_SD, "Card unmounted");
}
