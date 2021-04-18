#include "servoControl.h"

#include <stdlib.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

mcpwm_config_t pwm_config;

void servoControl_init() {
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, CONFIG_SERVOCONTROL_PIN);

    pwm_config.frequency = 1000000 / CONFIG_SERVOCONTROL_PERIOD_US;
    pwm_config.cmpr_a = 0;    	  //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;   	  //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
}

void servoControl_setPosition(int pos) {
	int pulse_width_us = pos * (CONFIG_SERVOCONTROL_WIDTH_MAX_US - CONFIG_SERVOCONTROL_WIDTH_MIN_US) / 100 + CONFIG_SERVOCONTROL_WIDTH_MIN_US;

	mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, pulse_width_us);
}
