################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include ThirdParty/SEGGER/SEGGER/subdir.mk
-include ThirdParty/SEGGER/OS/subdir.mk
-include ThirdParty/SEGGER/Config/subdir.mk
-include ThirdParty/FreeRTOS/org/Source/portable/MemMang/subdir.mk
-include ThirdParty/FreeRTOS/org/Source/portable/GCC/ARM_CM4F/subdir.mk
-include ThirdParty/FreeRTOS/org/Source/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Drivers/Modules/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := FreeRTOS_imu_servo_tek_eksende_denge_koruma
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
FreeRTOS_imu_servo_tek_eksende_denge_koruma.elf \

MAP_FILES += \
FreeRTOS_imu_servo_tek_eksende_denge_koruma.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
FreeRTOS_imu_servo_tek_eksende_denge_koruma.list \


# All Target
all: main-build

# Main-build Target
main-build: FreeRTOS_imu_servo_tek_eksende_denge_koruma.elf secondary-outputs

# Tool invocations
FreeRTOS_imu_servo_tek_eksende_denge_koruma.elf FreeRTOS_imu_servo_tek_eksende_denge_koruma.map: $(OBJS) $(USER_OBJS) /Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/STM32F411RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "FreeRTOS_imu_servo_tek_eksende_denge_koruma.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/Users/mehmet_dora/STM32CubeIDE/FreeRTOS_STM32/FreeRTOS_imu_servo_tek_eksende_denge_koruma/STM32F411RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="FreeRTOS_imu_servo_tek_eksende_denge_koruma.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

FreeRTOS_imu_servo_tek_eksende_denge_koruma.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "FreeRTOS_imu_servo_tek_eksende_denge_koruma.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) FreeRTOS_imu_servo_tek_eksende_denge_koruma.elf FreeRTOS_imu_servo_tek_eksende_denge_koruma.list FreeRTOS_imu_servo_tek_eksende_denge_koruma.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
