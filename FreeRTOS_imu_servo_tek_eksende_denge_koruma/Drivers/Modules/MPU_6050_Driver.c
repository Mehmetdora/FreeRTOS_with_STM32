/*
 * MPU_6050_Driver.c
 *
 *  Created on: Apr 3, 2026
 *      Author: mehmet_dora
 */

#include "MPU_6050_Driver.h"
#include "I2C_Driver.h"
#include "stm32f4xx.h"
#include "stdint.h"


#define MPU6050_ADDR  (0x68)
#define ACCEL_START   0x3B
#define GYRO_START    0x43

int16_t accelX, accelY, accelZ;
int16_t gyroX,  gyroY,  gyroZ;
uint8_t buffer[6];


void MPU_init(void){

	uint16_t PWR_MGMT_1 = 0x6B;
	// uykudan uyandırmak için gönderilecek değer : 0x00
	char wakeup = 0x00;
	I2C1_BurstWrite(MPU6050_ADDR, PWR_MGMT_1, 1, &wakeup);
}


void MPU6050_ReadAccel(float* accel) {
    I2C1_BurstRead(MPU6050_ADDR, ACCEL_START, 6, buffer);
    accelX = (buffer[0] << 8) | buffer[1];
    accelY = (buffer[2] << 8) | buffer[3];
    accelZ = (buffer[4] << 8) | buffer[5];

    accel[0] = accelX / 16384.0f;
    accel[1] = accelY / 16384.0f;
    accel[2] = accelZ / 16384.0f;

}

void MPU6050_ReadGyro(float* gyro) {
    I2C1_BurstRead(MPU6050_ADDR, GYRO_START, 6, buffer);
    gyroX = (buffer[0] << 8) | buffer[1];
    gyroY = (buffer[2] << 8) | buffer[3];
    gyroZ = (buffer[4] << 8) | buffer[5];

    gyro[0] = gyroX / 131.0f;
    gyro[1] = gyroY / 131.0f;
    gyro[2] = gyroZ / 131.0f;

}


float MPU6050_GetPitchAngle(void)
{
    float accel[3];

    MPU6050_ReadAccel(accel);

    float accelX = accel[0];
    float accelZ = accel[2];

    float pitch = -atan2f(accelX, accelZ) * 57.2958f;

    return pitch;
}


float NormalizeAngle180(float angle)
{
    while(angle > 180.0f)
    {
        angle -= 360.0f;
    }

    while(angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}


float MPU6050_GetRollAngle(void)
{
    float accel[3];

    MPU6050_ReadAccel(accel);

    float accelY = accel[1];
    float accelZ = accel[2];

    float roll = atan2f(accelY, accelZ) * 57.2958f;

    return roll;
}


