#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

/* SERCOCONTROL and MOTORCONTROL uses the same PWM generator,
 * they can probably interract in ways you don't want !!
 * E.g I suspect that their base frequency is common !
 */

void servoControl_init();

void servoControl_setPosition(int pos);

#endif