################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Modules/GPIO_Driver.c \
../Drivers/Modules/I2C_Driver.c \
../Drivers/Modules/MPU_6050_Driver.c \
../Drivers/Modules/TimerDriver.c \
../Drivers/Modules/UART_Driver.c 

OBJS += \
./Drivers/Modules/GPIO_Driver.o \
./Drivers/Modules/I2C_Driver.o \
./Drivers/Modules/MPU_6050_Driver.o \
./Drivers/Modules/TimerDriver.o \
./Drivers/Modules/UART_Driver.o 

C_DEPS += \
./Drivers/Modules/GPIO_Driver.d \
./Drivers/Modules/I2C_Driver.d \
./Drivers/Modules/MPU_6050_Driver.d \
./Drivers/Modules/TimerDriver.d \
./Drivers/Modules/UART_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Modules/%.o Drivers/Modules/%.su Drivers/Modules/%.cyclo: ../Drivers/Modules/%.c Drivers/Modules/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/ThirdParty/SEGGER/Config" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/Drivers/Modules" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/ThirdParty/FreeRTOS/org/Source/include" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/ThirdParty/FreeRTOS/org/Source" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/ThirdParty/FreeRTOS/org/Source/portable/MemMang" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I../Core/Inc -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/ThirdParty/FreeRTOS/org" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/ThirdParty/SEGGER/OS" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/ThirdParty/SEGGER/SEGGER" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Modules

clean-Drivers-2f-Modules:
	-$(RM) ./Drivers/Modules/GPIO_Driver.cyclo ./Drivers/Modules/GPIO_Driver.d ./Drivers/Modules/GPIO_Driver.o ./Drivers/Modules/GPIO_Driver.su ./Drivers/Modules/I2C_Driver.cyclo ./Drivers/Modules/I2C_Driver.d ./Drivers/Modules/I2C_Driver.o ./Drivers/Modules/I2C_Driver.su ./Drivers/Modules/MPU_6050_Driver.cyclo ./Drivers/Modules/MPU_6050_Driver.d ./Drivers/Modules/MPU_6050_Driver.o ./Drivers/Modules/MPU_6050_Driver.su ./Drivers/Modules/TimerDriver.cyclo ./Drivers/Modules/TimerDriver.d ./Drivers/Modules/TimerDriver.o ./Drivers/Modules/TimerDriver.su ./Drivers/Modules/UART_Driver.cyclo ./Drivers/Modules/UART_Driver.d ./Drivers/Modules/UART_Driver.o ./Drivers/Modules/UART_Driver.su

.PHONY: clean-Drivers-2f-Modules

