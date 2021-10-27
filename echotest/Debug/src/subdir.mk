################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/echo.c \
../src/iic_phyreset.c \
../src/main.c \
../src/webserver.c 

OBJS += \
./src/echo.o \
./src/iic_phyreset.o \
./src/main.o \
./src/webserver.o 

C_DEPS += \
./src/echo.d \
./src/iic_phyreset.d \
./src/main.d \
./src/webserver.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -IC:/Users/c.eguzo/Desktop/C-EGUZO-PHD-FZ/GENERAL/Udacity_project/ADC_Project/deletefolder/design_1_wrapper/export/design_1_wrapper/sw/design_1_wrapper/domain_ps7_cortexa9_0/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


