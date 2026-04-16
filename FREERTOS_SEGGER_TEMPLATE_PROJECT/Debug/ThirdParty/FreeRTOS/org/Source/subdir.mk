################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/FreeRTOS/org/Source/croutine.c \
../ThirdParty/FreeRTOS/org/Source/event_groups.c \
../ThirdParty/FreeRTOS/org/Source/list.c \
../ThirdParty/FreeRTOS/org/Source/queue.c \
../ThirdParty/FreeRTOS/org/Source/stream_buffer.c \
../ThirdParty/FreeRTOS/org/Source/tasks.c \
../ThirdParty/FreeRTOS/org/Source/timers.c 

OBJS += \
./ThirdParty/FreeRTOS/org/Source/croutine.o \
./ThirdParty/FreeRTOS/org/Source/event_groups.o \
./ThirdParty/FreeRTOS/org/Source/list.o \
./ThirdParty/FreeRTOS/org/Source/queue.o \
./ThirdParty/FreeRTOS/org/Source/stream_buffer.o \
./ThirdParty/FreeRTOS/org/Source/tasks.o \
./ThirdParty/FreeRTOS/org/Source/timers.o 

C_DEPS += \
./ThirdParty/FreeRTOS/org/Source/croutine.d \
./ThirdParty/FreeRTOS/org/Source/event_groups.d \
./ThirdParty/FreeRTOS/org/Source/list.d \
./ThirdParty/FreeRTOS/org/Source/queue.d \
./ThirdParty/FreeRTOS/org/Source/stream_buffer.d \
./ThirdParty/FreeRTOS/org/Source/tasks.d \
./ThirdParty/FreeRTOS/org/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/FreeRTOS/org/Source/%.o ThirdParty/FreeRTOS/org/Source/%.su ThirdParty/FreeRTOS/org/Source/%.cyclo: ../ThirdParty/FreeRTOS/org/Source/%.c ThirdParty/FreeRTOS/org/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I"/Users/mehmet_dora/Desktop/FreeRTOS - STM32/FREERTOS_SEGGER_TEMPLATE_PROJECT/ThirdParty/SEGGER/Config" -I"/Users/mehmet_dora/Desktop/FreeRTOS - STM32/FREERTOS_SEGGER_TEMPLATE_PROJECT/ThirdParty/FreeRTOS/org/Source/include" -I"/Users/mehmet_dora/Desktop/FreeRTOS - STM32/FREERTOS_SEGGER_TEMPLATE_PROJECT/ThirdParty/FreeRTOS/org/Source" -I"/Users/mehmet_dora/Desktop/FreeRTOS - STM32/FREERTOS_SEGGER_TEMPLATE_PROJECT/ThirdParty/FreeRTOS/org/Source/portable/MemMang" -I"/Users/mehmet_dora/Desktop/FreeRTOS - STM32/FREERTOS_SEGGER_TEMPLATE_PROJECT/ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F" -I../Core/Inc -I"/Users/mehmet_dora/Desktop/FreeRTOS - STM32/FREERTOS_SEGGER_TEMPLATE_PROJECT/ThirdParty/FreeRTOS/org" -I"/Users/mehmet_dora/Desktop/FreeRTOS - STM32/FREERTOS_SEGGER_TEMPLATE_PROJECT/ThirdParty/SEGGER/OS" -I"/Users/mehmet_dora/Desktop/FreeRTOS - STM32/FREERTOS_SEGGER_TEMPLATE_PROJECT/ThirdParty/SEGGER/SEGGER" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ThirdParty-2f-FreeRTOS-2f-org-2f-Source

clean-ThirdParty-2f-FreeRTOS-2f-org-2f-Source:
	-$(RM) ./ThirdParty/FreeRTOS/org/Source/croutine.cyclo ./ThirdParty/FreeRTOS/org/Source/croutine.d ./ThirdParty/FreeRTOS/org/Source/croutine.o ./ThirdParty/FreeRTOS/org/Source/croutine.su ./ThirdParty/FreeRTOS/org/Source/event_groups.cyclo ./ThirdParty/FreeRTOS/org/Source/event_groups.d ./ThirdParty/FreeRTOS/org/Source/event_groups.o ./ThirdParty/FreeRTOS/org/Source/event_groups.su ./ThirdParty/FreeRTOS/org/Source/list.cyclo ./ThirdParty/FreeRTOS/org/Source/list.d ./ThirdParty/FreeRTOS/org/Source/list.o ./ThirdParty/FreeRTOS/org/Source/list.su ./ThirdParty/FreeRTOS/org/Source/queue.cyclo ./ThirdParty/FreeRTOS/org/Source/queue.d ./ThirdParty/FreeRTOS/org/Source/queue.o ./ThirdParty/FreeRTOS/org/Source/queue.su ./ThirdParty/FreeRTOS/org/Source/stream_buffer.cyclo ./ThirdParty/FreeRTOS/org/Source/stream_buffer.d ./ThirdParty/FreeRTOS/org/Source/stream_buffer.o ./ThirdParty/FreeRTOS/org/Source/stream_buffer.su ./ThirdParty/FreeRTOS/org/Source/tasks.cyclo ./ThirdParty/FreeRTOS/org/Source/tasks.d ./ThirdParty/FreeRTOS/org/Source/tasks.o ./ThirdParty/FreeRTOS/org/Source/tasks.su ./ThirdParty/FreeRTOS/org/Source/timers.cyclo ./ThirdParty/FreeRTOS/org/Source/timers.d ./ThirdParty/FreeRTOS/org/Source/timers.o ./ThirdParty/FreeRTOS/org/Source/timers.su

.PHONY: clean-ThirdParty-2f-FreeRTOS-2f-org-2f-Source

