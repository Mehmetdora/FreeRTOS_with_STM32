################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/%.o ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/%.su ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/%.cyclo: ../ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/%.c ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/ThirdParty/SEGGER/Config" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/Drivers/Modules" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/ThirdParty/FreeRTOS/org/Source/include" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/ThirdParty/FreeRTOS/org/Source" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/ThirdParty/FreeRTOS/org/Source/portable/MemMang" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I../Core/Inc -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/ThirdParty/FreeRTOS/org" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/ThirdParty/SEGGER/OS" -I"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/Queue_management_2/ThirdParty/SEGGER/SEGGER" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ThirdParty-2f-FreeRTOS-2f-org-2f-Source-2f-portable-2f-GCC-2f-ARM_CM4F

clean-ThirdParty-2f-FreeRTOS-2f-org-2f-Source-2f-portable-2f-GCC-2f-ARM_CM4F:
	-$(RM) ./ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.cyclo ./ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.d ./ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.o ./ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/port.su

.PHONY: clean-ThirdParty-2f-FreeRTOS-2f-org-2f-Source-2f-portable-2f-GCC-2f-ARM_CM4F

