################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./bluetooth.o" \
"./main.o" \
"./motion_control.o" \
"./system_TM4C123.o" \
"./three_axes.o" \
"./tm4c123gh6pm_startup_ccs_gcc.o" \
"./x_axis.o" \
"./y_axis.o" \
"./z_axis.o" \
"./rtos/keys_driver.o" \
"./rtos/pulse_train.o" \
"./rtos/rtos.o" \
"./rtos/rtos_pipe.o" \
"./rtos/rtos_sched.o" \
"./rtos/rtos_task.o" \
"./rtos/uart_driver.o" \
-Wl,-T"../tm4c123gh6pm.lds" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lc \
-ldriver \
-lgcc \
-lnosys \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include rtos/subdir_vars.mk
-include subdir_rules.mk
-include rtos/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
three_axes.out \

EXE_OUTPUTS__QUOTED += \
"three_axes.out" \

BIN_OUTPUTS += \
three_axes.hex \

BIN_OUTPUTS__QUOTED += \
"three_axes.hex" \


# All Target
all: three_axes.out

# Tool invocations
three_axes.out: $(OBJS) $(LDS_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Linker'
	"C:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc.exe" -march=armv7e-m -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -DTM4C123GH6PM -DPART_TM4C123GH6PM -Dgcc -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -Wl,-Map,"three_axes.map" -L"C:/ti/TivaWare_C_Series-2.1.2.111/driverlib/gcc" -o"three_axes.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

three_axes.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: GNU Objcopy Utility'
	"C:/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-objcopy.exe"  $(EXE_OUTPUTS__QUOTED) "three_axes.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "bluetooth.d" "main.d" "motion_control.d" "system_TM4C123.d" "three_axes.d" "tm4c123gh6pm_startup_ccs_gcc.d" "x_axis.d" "y_axis.d" "z_axis.d" "rtos\keys_driver.d" "rtos\pulse_train.d" "rtos\rtos.d" "rtos\rtos_pipe.d" "rtos\rtos_sched.d" "rtos\rtos_task.d" "rtos\uart_driver.d" 
	-$(RM) "bluetooth.o" "main.o" "motion_control.o" "system_TM4C123.o" "three_axes.o" "tm4c123gh6pm_startup_ccs_gcc.o" "x_axis.o" "y_axis.o" "z_axis.o" "rtos\keys_driver.o" "rtos\pulse_train.o" "rtos\rtos.o" "rtos\rtos_pipe.o" "rtos\rtos_sched.o" "rtos\rtos_task.o" "rtos\uart_driver.o" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

