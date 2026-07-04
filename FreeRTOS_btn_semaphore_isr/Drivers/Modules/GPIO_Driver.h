/*
 * GPIO_Driver.h
 *
 *  Created on: Jun 28, 2026
 *      Author: mehmet_dora
 */

#ifndef MODULES_GPIO_DRIVER_H_
#define MODULES_GPIO_DRIVER_H_


#include "stdint.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"


void gpio_setup(void);
void gpio_set(uint8_t pin_num);
void gpio_reset(uint8_t pin_num);


#endif /* MODULES_GPIO_DRIVER_H_ */
