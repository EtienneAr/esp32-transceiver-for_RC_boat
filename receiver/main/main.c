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
#define CONTROL_WATCHDOG_TIMEOUT_CNT 250
#define CONTROL_FULLSPEED_DELAY_CNT 5000

static volatile float signal_quality;
static volatile int signal_quality_int; 

static void sensor_callback(void* arg) {
    wifi_send_data(&signal_quality_int, sizeof(int));
}

static volatile wifi_datagram_t control_data;
static volatile int control_watchdog = 0;
static float speed_filtered = 0;

static void control_callback(void* arg) {
    int speed = control_data.speed;
    int dir = control_data.dir;

    if(control_watchdog > CONTROL_WATCHDOG_TIMEOUT_CNT) {
        speed = 0;
    } else {
        control_watchdog++;
    }

    if( speed > 0) {
        speed = speed * control_data.limit_speed / 1000;   //[-1000;1000] => [-limit;limit]
    } else {
        speed = speed * 2*control_data.limit_speed / 1000; //[-1000;1000] => [-2limit;limit]
    }
    dir = 1000 - (dir+1000)/2;                        //[-1000;1000] => [0, 1000]

    speed = RANGE_CONST(speed, -1000, 1000);
    speed = RANGE_CONST(speed, -2*control_data.limit_speed, control_data.limit_speed);
    dir = RANGE_CONST(dir, 200, 800); //reduced range for turning

    // Filter speed
    float time_to_max = CONTROL_FULLSPEED_DELAY_CNT * (1000. - (float) control_data.limit_acc) / 1000.;

    float max_step = 1000.;
    if(time_to_max > 0)
        max_step = 1000. / time_to_max; //=> max / time_to_max

    if(speed > -10 && (float) speed > speed_filtered + max_step) { //Filter the speed only if going forward
        speed_filtered += max_step;
    } else if(speed > -10 && (float) speed < speed_filtered - max_step) { //Filter the speed only if going forward
        speed_filtered -= max_step;
    } else {
        speed_filtered = (float) speed;
    }

    motorControl_setSpeed((int) speed_filtered);
    servoControl_setPosition(dir);
}

static long lastDatagram_cnt = 0;

void receive_callback (uint8_t* src_mac[6], uint8_t *raw_data, int raw_len) {
    if(raw_len != sizeof(wifi_datagram_t)) 
        return;

    memcpy(&control_data, raw_data, sizeof(wifi_datagram_t));

    // Signal quality
    int instant_quality = (int) (((wifi_datagram_t*) raw_data)->cnt - lastDatagram_cnt) - 1;
    instant_quality = 1000 - instant_quality * 100; // 10 packets dropped => quality = 0

    if(instant_quality < 0) 
        instant_quality = 0;
    if(instant_quality > 1000)
        instant_quality = 1000;

    signal_quality = 0.95f * signal_quality + 0.05f * instant_quality;

    signal_quality_int = (int) signal_quality;
    lastDatagram_cnt = ((wifi_datagram_t*) raw_data)->cnt;

    // Timeout setup
    control_watchdog = 0;
}

void app_main(void)
{
    motorControl_init();
    servoControl_init();

    wifi_init();

    wifi_attach_recv_cb(&receive_callback);


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
