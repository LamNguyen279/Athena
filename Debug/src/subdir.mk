################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/vEvent.c \
../src/vScheduler.c \
../src/vTask.c 

OBJS += \
./src/main.o \
./src/vEvent.o \
./src/vScheduler.o \
./src/vTask.o 

C_DEPS += \
./src/main.d \
./src/vEvent.d \
./src/vScheduler.d \
./src/vTask.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


