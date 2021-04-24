/* UART asynchronous example, that uses separate RX and TX tasks
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#include "direct_wifi.h"

#include "motorControl.h"
#include "servoControl.h"
#include "wifi_datagram.h"

#define RANGE_CONST(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define SEND_PERIOD_MS 100

static int signal_quality; 

static void periodic_timer_callback(void* arg) {
    signal_quality = 50;
    wifi_send_data(&signal_quality, sizeof(int));
}

void receive_and_control_CB (uint8_t* src_mac[6], uint8_t *raw_data, int raw_len) {
    if(raw_len != sizeof(wifi_datagram_t)) 
        return;

    wifi_datagram_t *data = (wifi_datagram_t *) raw_data;

    data->speed = data->speed * data->limit_speed / 1000; //[-1000;1000] => [-limit;limit]
    data->dir = 1000 - (data->dir+1000)/2;                       //[-1000;1000] => [0, 1000]

    data->speed = RANGE_CONST(data->speed, -data->limit_speed, data->limit_speed);
    data->dir = RANGE_CONST(data->dir, 200, 800); //reduced range for turning

    motorControl_setSpeed(data->speed);
    servoControl_setPosition(data->dir);
}

void app_main(void)
{
    motorControl_init();
    servoControl_init();

    wifi_init();

    wifi_attach_recv_cb(&receive_and_control_CB);


    // Timer to send sensor data
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            .name = "wifi_send"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, SEND_PERIOD_MS * 1000));
}
