################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cpu_setup.c \
../src/e-bike_app.c \
../src/main.c \
../src/motor_control.c \
../src/pwm_isr.c 

OBJS += \
./src/cpu_setup.o \
./src/e-bike_app.o \
./src/main.o \
./src/motor_control.o \
./src/pwm_isr.o 

C_DEPS += \
./src/cpu_setup.d \
./src/e-bike_app.d \
./src/main.d \
./src/motor_control.d \
./src/pwm_isr.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/cpu_setup.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


