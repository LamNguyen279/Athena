################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Athesar/Glue/vOs/vEvent.c \
../Athesar/Glue/vOs/vScheduler.c \
../Athesar/Glue/vOs/vTask.c 

OBJS += \
./Athesar/Glue/vOs/vEvent.o \
./Athesar/Glue/vOs/vScheduler.o \
./Athesar/Glue/vOs/vTask.o 

C_DEPS += \
./Athesar/Glue/vOs/vEvent.d \
./Athesar/Glue/vOs/vScheduler.d \
./Athesar/Glue/vOs/vTask.d 


# Each subdirectory must supply rules for building sources it contributes
Athesar/Glue/vOs/%.o: ../Athesar/Glue/vOs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DSOAD_DEBUG -I"D:\Athena\Athena\Athesar\App" -I"D:\Athena\Athena\Athesar\Bsw\SoAd" -I"D:\Athena\Athena\Athesar\Bsw\DoIP" -I"D:\Athena\Athena\Athesar\Bsw\include" -I"D:\Athena\Athena\Athesar\Glue\vOs" -I"D:\Athena\Athena\Athesar\Bsw" -I"D:\Athena\Athena\Athesar\Cfg" -I"D:\Athena\Athena\Athesar\Glue" -I"D:\Athena\Athena\Athesar\Rte" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


