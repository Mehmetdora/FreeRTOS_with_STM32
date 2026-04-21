/*
 * LED_Driver.h
 *
 *  Created on: Apr 17, 2026
 *      Author: mehmet_dora
 */

#ifndef MODULES_LED_DRIVER_H_
#define MODULES_LED_DRIVER_H_

#include "stdint.h"

void LED_config(void);
void LED_ON(void);
void LED_OFF(void);
void LED_TOGGLE(void);
void LED_BLINK(uint16_t blink_ms);


#endif /* MODULES_LED_DRIVER_H_ */
