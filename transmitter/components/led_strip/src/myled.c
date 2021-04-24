#include "driver/rmt.h"
#include "led_strip.h"

#include "myled.h"

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define LED_NUMBER 1

static rmt_config_t config;
static led_strip_config_t strip_config;
static led_strip_t *p_strip;

void myled_init() {
    config.channel = RMT_TX_CHANNEL; // Channel 0
    config.gpio_num = CONFIG_LEDS_PIN; // GPIO
    config.clk_div = 2; // set counter clock to 40MHz

    //Default setup
	config.rmt_mode = RMT_MODE_TX;
    config.mem_block_num = 1;
        config.tx_config.carrier_freq_hz = 38000;
        config.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
        config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
        config.tx_config.carrier_duty_percent = 33;
        config.tx_config.carrier_en = false;
        config.tx_config.loop_en = false;
        config.tx_config.idle_output_en = true;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    strip_config.max_leds = LED_NUMBER;
    strip_config.dev = (led_strip_dev_t)config.channel;
    p_strip = led_strip_new_rmt_ws2812(&strip_config);
}

void myled_display_rg(int r_percent_g) {
	unsigned int r = 	  r_percent_g  * 255/100;
	unsigned int g = (100-r_percent_g) * 255/100;

	ESP_ERROR_CHECK(p_strip->set_pixel(p_strip, 0, r, g, 0));
    ESP_ERROR_CHECK(p_strip->refresh(p_strip, 100));
}