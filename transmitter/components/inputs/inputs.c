#include "inputs.h"
#include <driver/adc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static int trimJoyA = 0;
static int trimJoyB = 0;

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
    		avg_A += inputs_readJoyA() - INPUTS_VALUEMAX/2;
    		avg_B += inputs_readJoyB() - INPUTS_VALUEMAX/2;
    		vTaskDelay(10/portTICK_PERIOD_MS);
    	}

    	trimJoyA = avg_A/64;
    	trimJoyB = avg_B/64;
    }
}

int inputs_readJoyA() {
	return adc1_get_raw(ADC1_CHANNEL_6) - trimJoyA;
}

int inputs_readJoyB() {
	return adc1_get_raw(ADC1_CHANNEL_7) - trimJoyB;
}

int inputs_readPotA() {
    return adc1_get_raw(ADC1_CHANNEL_4);
}

int inputs_readPotB() {
    return adc1_get_raw(ADC1_CHANNEL_5);
}