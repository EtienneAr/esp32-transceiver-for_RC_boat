#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"

#include "direct_wifi.h"
#include "screen_handler.h"
#include "sticks.h"
#include "wifi_datagram.h"
#include "myled.h"

#include <math.h>

#define WIFI_TIMEOUT 10000000

static const char *MAIN_TAG = "RC-Test main";


static void wifi_recv_cb(uint8_t src_mac[6], uint8_t *data, int len) {
    
}

void app_main(void)
{

    sticks_init(true); //auto trim
    wifi_init();
    myled_init();

    wifi_attach_recv_cb(wifi_recv_cb);

    wifi_datagram_t data;
    data.cnt = 0;

    while(true) {
        data.cnt++;
        data.speed = sticks_readJoyA() * 2000 / STICKS_VALUEMAX - 1000;
        data.dir   = sticks_readJoyB() * 2000 / STICKS_VALUEMAX - 1000;
        data.limit_speed = sticks_readPotA() * 1000 / STICKS_VALUEMAX;
        data.limit_acc   = sticks_readPotB() * 1000 / STICKS_VALUEMAX;

        wifi_datagram_print(&data);

        myled_display_rg((data.cnt/10)%100);

        wifi_send_data(&data, sizeof(wifi_datagram_t));
        vTaskDelay(1 + 1/portTICK_PERIOD_MS);
    }
}