################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BaseDriver/Adc.c \
../BaseDriver/Gpio.c \
../BaseDriver/Timer.c \
../BaseDriver/lpuart_LPUART_0.c \
../BaseDriver/peripheral_init.c 

OBJS += \
./BaseDriver/Adc.o \
./BaseDriver/Gpio.o \
./BaseDriver/Timer.o \
./BaseDriver/lpuart_LPUART_0.o \
./BaseDriver/peripheral_init.o 

C_DEPS += \
./BaseDriver/Adc.d \
./BaseDriver/Gpio.d \
./BaseDriver/Timer.d \
./BaseDriver/lpuart_LPUART_0.d \
./BaseDriver/peripheral_init.d 


# Each subdirectory must supply rules for building sources it contributes
BaseDriver/%.o: ../BaseDriver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@BaseDriver/Adc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


