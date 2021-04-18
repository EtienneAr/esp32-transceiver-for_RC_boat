#include "sticks.h"
#include <driver/adc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static int trimJoyA = 0;
static int trimJoyB = 0;

void sticks_init(bool autoTrimJoy) {
    adc1_config_width(ADC_WIDTH_BIT_11);
	
	// JoyA stick
    adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);
    
    // JoyB stick
    adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11);

    // PotA stick
    adc1_config_channel_atten(ADC1_CHANNEL_4,ADC_ATTEN_DB_11);

    if(autoTrimJoy) {
    	vTaskDelay(10/portTICK_PERIOD_MS);
    	int avg_A = 0, avg_B = 0;

    	for(int i=0;i<64;i++) {
    		avg_A += sticks_readJoyA() - STICKS_VALUEMAX/2;
    		avg_B += sticks_readJoyB() - STICKS_VALUEMAX/2;
    		vTaskDelay(10/portTICK_PERIOD_MS);
    	}

    	trimJoyA = avg_A/64;
    	trimJoyB = avg_B/64;
    }
}

int sticks_readJoyA() {
	return adc1_get_raw(ADC1_CHANNEL_6) - trimJoyA;
}

int sticks_readJoyB() {
	return adc1_get_raw(ADC1_CHANNEL_7) - trimJoyB;
}

int sticks_readPotA() {
    return adc1_get_raw(ADC1_CHANNEL_4);
}