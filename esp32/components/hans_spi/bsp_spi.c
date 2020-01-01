#include "bsp_spi.h"


bool bsp_spi_init(int miso_io, int mosi_io, int sck_io,int max_transfer_sz) {
	const bool if_init = 0;

	spi_host_device_t spi_host = VSPI_HOST;
	#ifdef CONFIG_HOST_HSPI
	spi_host = HSPI_HOST;
	#endif
	if (if_init == 1)
		return 0;
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = miso_io,
        .mosi_io_num = mosi_io,
        .sclk_io_num = sck_io,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = max_transfer_sz
        };
    ret = spi_bus_initialize(spi_host, &buscfg, 0);
    ESP_ERROR_CHECK(ret);
    return ret;
}