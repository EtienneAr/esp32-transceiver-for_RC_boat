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
#define CONTROL_PERIOD_MS 1

static int signal_quality; 

static void sensor_callback(void* arg) {
    signal_quality = 50;
    wifi_send_data(&signal_quality, sizeof(int));
}

static wifi_datagram_t last_datagram;

static void control_callback(void* arg) {
    int speed = last_datagram.speed;
    int dir = last_datagram.dir;

    speed = speed * last_datagram.limit_speed / 1000; //[-1000;1000] => [-limit;limit]
    dir = 1000 - (dir+1000)/2;                        //[-1000;1000] => [0, 1000]

    speed = RANGE_CONST(speed, -last_datagram.limit_speed, last_datagram.limit_speed);
    dir = RANGE_CONST(dir, 200, 800); //reduced range for turning

    motorControl_setSpeed(speed);
    servoControl_setPosition(dir);
}


void receive_and_control_CB (uint8_t* src_mac[6], uint8_t *raw_data, int raw_len) {
    if(raw_len != sizeof(wifi_datagram_t)) 
        return;

    memcpy(&last_datagram, raw_data, sizeof(wifi_datagram_t));
}

void app_main(void)
{
    motorControl_init();
    servoControl_init();

    wifi_init();

    wifi_attach_recv_cb(&receive_and_control_CB);


    // Timer to send sensor data
    const esp_timer_create_args_t sensor_periodic_timer_args = {
            .callback = &sensor_callback,
            .name = "sensor_callback"
    };
    esp_timer_handle_t sensor_periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&sensor_periodic_timer_args, &sensor_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(sensor_periodic_timer, SEND_PERIOD_MS * 1000));

    // Car control
    const esp_timer_create_args_t control_periodic_timer_args = {
            .callback = &control_callback,
            .name = "sensor_callback"
    };
    esp_timer_handle_t control_periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&control_periodic_timer_args, &control_periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(control_periodic_timer, CONTROL_PERIOD_MS * 1000));
}
