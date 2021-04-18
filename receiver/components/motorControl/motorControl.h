#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

/* SERCOCONTROL and MOTORCONTROL uses the same PWM generator,
 * they can probably interract in ways you don't want !!
 * E.g I suspect that their base frequency is common !
 */

void motorControl_init();

void motorControl_setSpeed(int speed);

#endif