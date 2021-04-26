#include "inputs.h"
#include <driver/adc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static int trimJoyA = INPUTS_VALUEMAX/2;
static int trimJoyB = INPUTS_VALUEMAX/2;

static int _inputs_readJoyA_raw();
static int _inputs_readJoyB_raw();

void inputs_init(bool autoTrimJoy) {
    adc1_config_width(ADC_WIDTH_BIT_11);
	
	// JoyA stick
    adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);
    
    // JoyB stick
    adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11);

    // PotA stick
    adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_11);

    // PotB stick
    adc1_config_channel_atten(ADC1_CHANNEL_5,ADC_ATTEN_DB_11);

    if(autoTrimJoy) {
    	vTaskDelay(10/portTICK_PERIOD_MS);
    	int avg_A = 0, avg_B = 0;

    	for(int i=0;i<64;i++) {
    		avg_A += _inputs_readJoyA_raw();
    		avg_B += _inputs_readJoyB_raw();
    		vTaskDelay(10/portTICK_PERIOD_MS);
    	}

    	trimJoyA = (float) avg_A/64;
    	trimJoyB = (float) avg_B/64;
    }

    // Button A
    gpio_set_direction(CONFIG_BUTTON_A_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(CONFIG_BUTTON_A_PIN);

    // Button B
    gpio_set_direction(CONFIG_BUTTON_B_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(CONFIG_BUTTON_B_PIN);
}

static int _inputs_readJoyA_raw() {
    return adc1_get_raw(ADC1_CHANNEL_6);
}

float inputs_readJoyA() {
    float val = (float) _inputs_readJoyA_raw();
    if(val > trimJoyA) {
        val = (1. + ((val - trimJoyA) / (INPUTS_VALUEMAX - trimJoyA))) * INPUTS_VALUEMAX / 2.;
    } else {
        val = (1. + ((val - trimJoyA) / trimJoyA)) * INPUTS_VALUEMAX / 2.;
    }
	return  val;
}

static int _inputs_readJoyB_raw() {
    return adc1_get_raw(ADC1_CHANNEL_7);
}

float inputs_readJoyB() {
	float val = (float) _inputs_readJoyB_raw();
    if(val > trimJoyB) {
        val = (1. + (val - trimJoyB) / (INPUTS_VALUEMAX - trimJoyB)) * INPUTS_VALUEMAX / 2.;
    } else {
        val = (1. + (val - trimJoyB) / trimJoyB) * INPUTS_VALUEMAX / 2.;
    }
    return  val;
}

int inputs_readPotA() {
    return adc1_get_raw(ADC1_CHANNEL_4);
}

int inputs_readPotB() {
    return adc1_get_raw(ADC1_CHANNEL_5);
}

bool inputs_readButtonA() {
    return (gpio_get_level(CONFIG_BUTTON_A_PIN) == 0);
}
bool inputs_readButtonB() {
    return (gpio_get_level(CONFIG_BUTTON_B_PIN) == 0);
}