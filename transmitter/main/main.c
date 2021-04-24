#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"

#include "direct_wifi.h"
#include "screen_handler.h"
#include "inputs.h"
#include "wifi_datagram.h"
#include "myleds.h"

#include <math.h>

#define WIFI_TIMEOUT 10000000

static const char *MAIN_TAG = "RC-Test main";


static void wifi_recv_cb(uint8_t src_mac[6], uint8_t *data, int len) {
    if(len != sizeof(int))
            return;

    int signal_quality = *((int*) data);

    if(signal_quality <=1000 && signal_quality >= 0)
        myleds_display_rg(signal_quality/10);
}

void app_main(void)
{

    inputs_init(true); //auto trim
    wifi_init();
    myleds_init();

    wifi_attach_recv_cb(wifi_recv_cb);

    wifi_datagram_t data;
    data.cnt = 0;

    while(true) {
        data.cnt++;
        data.speed = inputs_readJoyA() * 2000 / INPUTS_VALUEMAX - 1000;
        data.dir   = inputs_readJoyB() * 2000 / INPUTS_VALUEMAX - 1000;
        data.limit_speed = inputs_readPotA() * 1000 / INPUTS_VALUEMAX;
        data.limit_acc   = inputs_readPotB() * 1000 / INPUTS_VALUEMAX;

        myleds_display_A(inputs_readButtonA());
        myleds_display_B(inputs_readButtonB());

        wifi_datagram_print(&data);

        wifi_send_data(&data, sizeof(wifi_datagram_t));
        vTaskDelay(1 + 1/portTICK_PERIOD_MS);
    }
}