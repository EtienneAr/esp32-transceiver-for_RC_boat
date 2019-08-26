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

static const int RX_BUF_SIZE = 1024;



#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_34)

unsigned long date, mytime, age;

struct GPS_data {
    bool isGPS;
    unsigned long time;
    float lat, lon;
    float head;
};

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

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        for(int i=0;i<rxBytes;i++) {
        	gps_encode((char) data[i]);
        }

        unsigned long _date, _mytime, _age;
        gps_get_datetime(&_date, &_mytime, &_age);
        if(date != _date || mytime != _mytime || age != _age) {
        	date = _date;
        	mytime = _mytime;
        	age = _age;

        	struct GPS_data pos;
            float temp_age;

        	gps_f_get_position(&(pos.lat), &(pos.lon), &temp_age);
            pos.head = COMPASS_getAngle();;
            pos.isGPS = temp_age != 0;
            pos.time = mytime;

            wifi_send_data(&pos, sizeof(struct GPS_data));
        }

        /*
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }
        */
    }
    free(data);
}

void app_main(void)
{
    GPS_init();

    wifi_init();

    COMPASS_init();

    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}
