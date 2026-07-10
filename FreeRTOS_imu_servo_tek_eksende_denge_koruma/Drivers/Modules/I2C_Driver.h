/*
 * I2C_Driver.h
 *
 *  Created on: Jul 7, 2026
 *      Author: mehmet_dora
 */

#ifndef MODULES_I2C_DRIVER_H_
#define MODULES_I2C_DRIVER_H_

#include "stdint.h"

void I2C1_init(void);
void I2C1_BurstRead(char saddr, char maddr, int n, uint8_t* data);
void I2C1_BurstWrite(char saddr, char maddr, int n, uint8_t* data);


#endif /* MODULES_I2C_DRIVER_H_ */
