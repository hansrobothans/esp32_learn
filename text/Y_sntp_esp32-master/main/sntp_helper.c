#include <time.h>
#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"
#include "esp_system.h"
#include "sntp_helper.h"
#include "esp_log.h"

static const char *TAG = "SNTP";
static sntp_on_connected_cb_t on_connected_cb = NULL;

/**
 * @brief Create a new event loop.
 *
 * @param[in] event_loop_args configuration structure for the event loop to create
 * @param[out] event_loop handle to the created event loop
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - ESP_FAIL: Failed to create task loop
 *  - Others: Fail
 */
void _time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
    on_connected_cb();
}

/**
 * @brief Create a new event loop.
 *
 * @param[in] event_loop_args configuration structure for the event loop to create
 * @param[out] event_loop handle to the created event loop
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - ESP_FAIL: Failed to create task loop
 *  - Others: Fail
 */
void sntp_set_on_connected_cb(sntp_on_connected_cb_t cb)
{
    on_connected_cb = cb;
}

/**
 * A brief history of JavaDoc-style (C-style) comments.
 *
 * This is the typical JavaDoc-style C-style comment. It starts with two
 * asterisks.
 *
 * @param void 
 * @sa https://www.timecalculator.net/milliseconds-to-date
 */
uint64_t sntp_get_sec_since_epoch(void)
{
    uint64_t secondsSinceEpoch = 0; // return value in milliseconds
    struct timeval tv; // struct timeval {
                       // time_t      tv_sec;     /* seconds */
                       // suseconds_t tv_usec;    /* microseconds */
                       // };

    gettimeofday(&tv, NULL); //  and gives the number of seconds and microseconds since the Epoch (see
                             // time(2)).  The tz argument is a struct timezone:

    secondsSinceEpoch = (uint64_t)(tv.tv_sec); 

    // printf("%llu\n", secondsSinceEpoch); // TODO: debug delete
    return secondsSinceEpoch;
}

/**
 * A brief history of JavaDoc-style (C-style) comments.
 *
 * This is the typical JavaDoc-style C-style comment. It starts with two
 * asterisks.
 *
 * @param void 
 * @sa https://www.timecalculator.net/milliseconds-to-date
 */   
uint64_t sntp_get_ms_since_epoch(void)
{
    uint64_t millisecondsSinceEpoch = 0; // return value in milliseconds
    struct timeval tv; // struct timeval {
                       // time_t      tv_sec;     /* seconds */
                       // suseconds_t tv_usec;    /* microseconds */
                       // };

    gettimeofday(&tv, NULL); //  and gives the number of seconds and microseconds since the Epoch (see
                             // time(2)).  The tz argument is a struct timezone:

    millisecondsSinceEpoch =
            (uint64_t)(tv.tv_sec) * 1000 +
            (uint64_t)(tv.tv_usec) / 1000;

    // printf("%llu\n", millisecondsSinceEpoch); // TODO: debug delete
    return millisecondsSinceEpoch;
}

/**
 * @brief Create a new event loop.
 *
 * @param[in] event_loop_args configuration structure for the event loop to create
 * @param[out] event_loop handle to the created event loop
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - ESP_FAIL: Failed to create task loop
 *  - Others: Fail
 */
esp_err_t sntp_get_tz_timeinfo(const char* tz, struct tm *timeinfo) 
{
    time_t now;
    // struct tm timeinfo;
    time(&now);
    setenv("TZ", tz, 1);
    tzset();
    localtime_r(&now, timeinfo);
    return ESP_OK;
}

/**
 * @brief Create a new event loop.
 *
 * @param[in] event_loop_args configuration structure for the event loop to create
 * @param[out] event_loop handle to the created event loop
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - ESP_FAIL: Failed to create task loop
 *  - Others: Fail
 */
esp_err_t sntp_helper_init(const char* sntp_pool_uri)
{
    int retry = 0;
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, sntp_pool_uri); // connect to ntp server pool via URI
    sntp_set_time_sync_notification_cb(_time_sync_notification_cb); // set the cb function linked in main
    sntp_init(); //Initialize this sntp module.

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < RETRY_ATTEMPTS) 
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, RETRY_ATTEMPTS);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        if(retry >= RETRY_ATTEMPTS)
        {
            return ESP_FAIL; // Couldn't connect, check LAN connection
        }
    }
    
    return ESP_OK; // connection success
}