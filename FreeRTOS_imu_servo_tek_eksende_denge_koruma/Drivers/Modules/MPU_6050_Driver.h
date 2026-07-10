/*
 * MPU_6050_Driver.h
 *
 *  Created on: Apr 3, 2026
 *      Author: mehmet_dora
 */

#ifndef MODULES_MPU_6050_DRIVER_H_
#define MODULES_MPU_6050_DRIVER_H_

#include "stdint.h"
#include "math.h"

void MPU_init(void);
void MPU6050_ReadAccel(float* accel);
void MPU6050_ReadGyro(float* gyro);
float MPU6050_GetRollAngle(void);
float MPU6050_GetPitchAngle(void);
float NormalizeAngle180(float angle);


#endif /* MODULES_MPU_6050_DRIVER_H_ */
