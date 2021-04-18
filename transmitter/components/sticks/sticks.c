#include "sticks.h"
#include <driver/adc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static int trimA = 0;
static int trimB = 0;

void sticks_init(bool autoTrimOnCenter) {
    adc1_config_width(ADC_WIDTH_BIT_11);
	
	// A stick
    adc1_config_channel_atten(ADC1_CHANNEL_6,ADC_ATTEN_DB_11);
    
    //B stick
    adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11);

    if(autoTrimOnCenter) {
    	vTaskDelay(10/portTICK_PERIOD_MS);
    	long avg_A = 0L, avg_B = 0L;

    	for(int i=0;i<50;i++) {
    		avg_A += (long) (sticks_readA() - (STICKS_VALUEMAX+1)/2);
    		avg_B += (long) (sticks_readB() - (STICKS_VALUEMAX+1)/2);
    		vTaskDelay(10/portTICK_PERIOD_MS);
    	}

    	trimA = (int) (avg_A/50);
    	trimB = (int) (avg_B/50);
    }
}

int sticks_readA() {
	return adc1_get_raw(ADC1_CHANNEL_6) - trimA;
}

int sticks_readB() {
	return adc1_get_raw(ADC1_CHANNEL_7) - trimB;
}