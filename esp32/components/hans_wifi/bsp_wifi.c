#include "bsp_wifi.h"


EventGroupHandle_t bsp_wifi_event_group;                     //wifi建立成功信号量

static int wifi_retry_num = 0;                 //重新连接的次数



static void smartconfig_task(void * parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
    while (1) {
        uxBits = xEventGroupWaitBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY); 
        if(uxBits & WIFI_CONNECTED_BIT) {
            ESP_LOGI(TAG_WIFI, "WiFi Connected to ap");
        }
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG_WIFI, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}



/*
* wifi 事件
* @param[in]   void                :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)              //STA模式-开始连接
    {
        #if SMARTCONFIG
            //smart配网
            xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
        #else
            //直接配网
            esp_wifi_connect();
        #endif
        
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)  //STA模式-断线
    {
        if (wifi_retry_num < BSP_ESP_STA_MAXIMUM_RETRY) 
        {
            esp_wifi_connect();
            xEventGroupClearBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT);
            wifi_retry_num++;
            ESP_LOGI(TAG_WIFI, "retry to connect to the AP");
        }
        ESP_LOGI(TAG_WIFI,"connect to the AP fail");
    } 
    // //未解决代码
    // case SYSTEM_EVENT_STA_CONNECTED:    //STA模式-连接成功
    //     xEventGroupSetBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT);
    //     break;
    // else if (event_id == SYSTEM_EVENT_STA_STACONNECTED)   //STA模式-连接成功
    // {
    //     wifi_event_ap_staconnected_t* event_sta_staconnected = (wifi_event_ap_staconnected_t*) event_data;
    //     ESP_LOGI(TAG_WIFI, "connect "MACSTR" join, AID=%d",
    //              MAC2STR(event_sta_staconnected->mac),
    //                     event_sta_staconnected->aid);
    //     xEventGroupSetBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT);
    // } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)            //STA模式-获取IP 
    {
        ip_event_got_ip_t* event_ip = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG_WIFI, "got ip:%s",
                 ip4addr_ntoa(&event_ip->ip_info.ip));
        wifi_retry_num = 0;
        xEventGroupSetBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)   //AP模式-有STA连接成功
    {
        wifi_event_ap_staconnected_t* event_ap_staconnected = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG_WIFI, "station "MACSTR" join, AID=%d",
                 MAC2STR(event_ap_staconnected->mac),
                        event_ap_staconnected->aid);
        xEventGroupSetBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT);
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) //AP模式-有STA断线
    {
        wifi_event_ap_stadisconnected_t* event_ap_stadisconnected = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG_WIFI, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event_ap_stadisconnected->mac), 
                        event_ap_stadisconnected->aid);
        xEventGroupClearBits(bsp_wifi_event_group, WIFI_CONNECTED_BIT);
    }
    // smart配网
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) 
    {
        ESP_LOGI(TAG_WIFI, "Scan done");
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) 
    {
        ESP_LOGI(TAG_WIFI, "Found channel");
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) 
    {
        ESP_LOGI(TAG_WIFI, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG_WIFI, "SSID:%s", ssid);
        ESP_LOGI(TAG_WIFI, "PASSWORD:%s", password);

        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
        ESP_ERROR_CHECK( esp_wifi_connect() );
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(bsp_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}



/*
* WIFI作为STA的初始化新版发布消息
* @param[in]   void                :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
void bsp_wifi_init_station(void)
{

    bsp_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
#if SMARTCONFIG
    //smart配网
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL) );
#else
    //提前输入密码配网
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = BSP_ESP_STA_WIFI_SSID,           //STA账号
            .password = BSP_ESP_STA_WIFI_PASSWORD},       //STA密码
    }; 

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
#endif
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG_WIFI, "wifi_init_sta finished.");
    ESP_LOGI(TAG_WIFI, "connect to ap SSID:%s password:%s \n",
             BSP_ESP_STA_WIFI_SSID, BSP_ESP_STA_WIFI_PASSWORD);
}
/*
* WIFI作为AP的初始化
* @param[in]   void                :无
* @retval      void                :无
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
void bsp_wifi_init_softap(void)
{
    tcpip_adapter_init();

    bsp_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = BSP_ESP_AP_WIFI_SSID,
            .password = BSP_ESP_AP_WIFI_PASSWORD,
            .ssid_len = 0,
            .max_connection = BSP_ESP_AP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };
    if (strlen(BSP_ESP_AP_WIFI_PASSWORD) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG_WIFI, "SoftAP set finish,SSID:%s password:%s \n",
             wifi_config.ap.ssid, wifi_config.ap.password);
}
/*
* 根据参数chr['a'],['s']选择sta或者ap模式
* @param[in]   char                :选择模式
* @retval      char                :成功直接返回传入参数，否则返回NULL
* @note        修改日志 
*               Ver0.0.1:
                    hx-zsj, 2018/08/06, 初始化版本\n 
*/
char bsp_wifi_init_char_station_softap(char chr)
{
    if(chr == 'a')
        bsp_wifi_init_softap();
    else if(chr == 's')
        bsp_wifi_init_station();
    else//参数错误
        return "e";
    return chr;

}

