#include "motorControl.h"

#include <stdlib.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

mcpwm_config_t pwm_config;

void motorControl_init() {
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, CONFIG_MOTORCONTROL_PIN);

    pwm_config.frequency = 1000000 / CONFIG_MOTORCONTROL_PERIOD_US;
    pwm_config.cmpr_a = 0;    	  //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;   	  //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
}

void motorControl_setSpeed(int speed) {
	int pulse_width_us = CONFIG_MOTORCONTROL_WIDTH_MID_US;
	if(speed > 0) {
		pulse_width_us =  speed * (CONFIG_MOTORCONTROL_WIDTH_MAX_US - CONFIG_MOTORCONTROL_WIDTH_MID_US) / 100 + CONFIG_MOTORCONTROL_WIDTH_MID_US;
	} else {
		pulse_width_us = -speed * (CONFIG_MOTORCONTROL_WIDTH_MIN_US - CONFIG_MOTORCONTROL_WIDTH_MID_US) / 100 + CONFIG_MOTORCONTROL_WIDTH_MID_US;
	}

	mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, pulse_width_us);
}