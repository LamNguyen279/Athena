################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Athesar/App/Boot/Main.c 

OBJS += \
./Athesar/App/Boot/Main.o 

C_DEPS += \
./Athesar/App/Boot/Main.d 


# Each subdirectory must supply rules for building sources it contributes
Athesar/App/Boot/%.o: ../Athesar/App/Boot/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"D:\Athena\Athena\Athesar\App" -I"D:\Athena\Athena\Athesar\Bsw\DoIP" -I"D:\Athena\Athena\Athesar\Bsw\include" -I"D:\Athena\Athena\Athesar\Glue\vOs" -I"D:\Athena\Athena\Athesar\Bsw" -I"D:\Athena\Athena\Athesar\Cfg" -I"D:\Athena\Athena\Athesar\Glue" -I"D:\Athena\Athena\Athesar\Rte" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


