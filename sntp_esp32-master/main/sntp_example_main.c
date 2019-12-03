/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/timers.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include <esp_system.h>
#include "protocol_examples_common.h"
#include "sntp_helper.h"

static const char *TAG = "main";

typedef enum {
    EVENT_TYPE_SNTP_CONNECTED,
    EVENT_TYPE_SNTP_DISCONNECTED,
} event_type_t;

static void _sntp_on_connected(void)
{
    // event_t *event = malloc(sizeof(*event));

    // event->type = EVENT_TYPE_SNTP_CONNECTED;

    // ESP_LOGD(TAG, "Queuing event WIFI_CONNECTED");
    // xQueueSend(event_queue, &event, portMAX_DELAY);
    ESP_LOGI(TAG,"SNTP CONNECTED");
}


/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR static int boot_count = 0;


void app_main()
{
    struct tm timeinfo;
    char strftime_buf[64];

    /* all template trash */
    ESP_ERROR_CHECK( nvs_flash_init() );
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_create_default() );

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing d
     * Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* utilize callback functionality to drive event cb */
    sntp_set_on_connected_cb(_sntp_on_connected);

    /* sntp_helper_init to connect to SNTP service and do 
     * a connection check to ensure status
     * is OK.
     */
    ESP_ERROR_CHECK(sntp_helper_init("pool.ntp.org")); 

    while(1)
    {  
        printf("milliseconds: %llu\n",sntp_get_ms_since_epoch());
        printf("seconds: %llu \n", sntp_get_sec_since_epoch());
        
        sntp_get_tz_timeinfo("EST5EDT,M3.2.0/2,M11.1.0", &timeinfo);
        
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(TAG, "The current date/time : %s", strftime_buf);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
