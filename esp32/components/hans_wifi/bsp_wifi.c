
#include "bsp_wifi.h"

static const char *TAG = "wifi softAP";
/** 
 * wifi事件处理函数
 * @param[in]   arg     :表示传入的事件类型对应所携带的参数
 * @param[in]   event_data   :表示传入的事件类型
 *              其他    :失败 
 * @par         修改日志 
 *               Ver0.0.1:
                     Helon_Chan, 2018/06/04, 初始化版本\n 
 */

static void bsp_wifi_softap_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) 
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } 
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) 
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

/** 
 * 应用程序的函数入口
 * @param[in]   NULL
 * @retval      NULL              
 * @par         修改日志 
 *               Ver0.0.1:
                     Helon_Chan, 2018/06/04, 初始化版本\n 
 */
void bsp_wifi_init_softap(void)
{
    tcpip_adapter_init();

    // ESP_ERROR_CHECK(esp_event_loop_init(bsp_wifi_softap_event_loop_handler, NULL));
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &bsp_wifi_softap_event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = BSP_ESP_AP_WIFI_SSID,
            .ssid_len = strlen(BSP_ESP_AP_WIFI_SSID),
             // 最多只能被4个station同时连接,这里设置为只能被一个station连接 
            .max_connection = BSP_ESP_AP_MAX_STA_CONN,
            .password = BSP_ESP_AP_WIFI_PASSWORD,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    if (strlen(BSP_ESP_AP_WIFI_PASSWORD) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());    
    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             BSP_ESP_AP_WIFI_SSID, BSP_ESP_AP_WIFI_PASSWORD);
}


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event 
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

// static const char *TAG = "wifi station";

static int s_retry_num = 0;

static void bsp_wifi_station_event_handler(void* arg, esp_event_base_t event_base, 
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < BSP_ESP_STA_MAXIMUM_RETRY) {
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:%s",
                 ip4addr_ntoa(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void bsp_wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &bsp_wifi_station_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &bsp_wifi_station_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = BSP_ESP_STA_WIFI_SSID,
            .password = BSP_ESP_STA_WIFI_PASSWORD
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
             BSP_ESP_STA_WIFI_SSID, BSP_ESP_STA_WIFI_PASSWORD);
}


