/*
 * TimerDriver.h
 *
 *  Created on: Jul 7, 2026
 *      Author: mehmet_dora
 */

#ifndef MODULES_TIMERDRIVER_H_
#define MODULES_TIMERDRIVER_H_

#include "stdint.h"


void PWMDriver_on(void);
void PWMDriver_off(void);
void PWMDriver_init(void);

void Servo_SetAngle(float angle);
void Servo_SetNeutral(void);
void Servo_SetPulseUs(uint16_t pulse_us);
float clamp_float(float value, float min, float max);
float limit_step(float target, float current, float max_step);


#endif /* MODULES_TIMERDRIVER_H_ */
