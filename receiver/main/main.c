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
#include "tinygps.h"

#include "compass_handler.hpp"
#include "motorControl.h"
#include "servoControl.h"

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_34)

#define SEND_PERIOD_MS 100

unsigned long date, mytime, age;

struct GPS_data {
    int GPS_count;
    bool isLost;
    unsigned long time;
    float lat, lon;
    float head;
};

static int GPS_count = 0;
static bool GPS_fixed = false;

void GPS_init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}

static void position_task(void *arg)
{
    uint8_t* ser_data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    unsigned long temp_age;

    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, ser_data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        
        for(int i=0;i<rxBytes;i++) {
        	if(gps_encode((char) ser_data[i])){
                GPS_count++;
                gps_get_position(NULL , NULL, &temp_age);
                GPS_fixed = temp_age == 0;  
            } 
        }
    }
    free(ser_data);
}

static void periodic_timer_callback(void* arg) {
        /* Retreive new position */
        struct GPS_data pos;
        unsigned long temp_age;

        gps_f_get_position(&(pos.lat), &(pos.lon), &temp_age);
        gps_get_datetime(NULL, &(pos.time), NULL);
        
        pos.head = COMPASS_getAngle();
        pos.GPS_count = GPS_count;
        pos.isLost = !GPS_fixed;

        wifi_send_data(&pos, sizeof(struct GPS_data));
}

void app_main(void)
{

    /*
    GPS_init();

    wifi_init();

    COMPASS_init();

    xTaskCreate(position_task, "uart_rx_task", 1024*2, NULL, tskIDLE_PRIORITY, NULL);

    // Timer to send sensor data
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            .name = "wifi_send"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, SEND_PERIOD_MS * 1000));
    */


    motorControl_init();
    servoControl_init();
    
    motorControl_setSpeed(0);

    while(true) {
        servoControl_setPosition(0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        servoControl_setPosition(20);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        servoControl_setPosition(40);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        servoControl_setPosition(60);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        servoControl_setPosition(80);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        servoControl_setPosition(100);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
