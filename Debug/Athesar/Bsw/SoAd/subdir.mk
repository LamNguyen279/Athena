################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Athesar/Bsw/SoAd/SoAd.c 

OBJS += \
./Athesar/Bsw/SoAd/SoAd.o 

C_DEPS += \
./Athesar/Bsw/SoAd/SoAd.d 


# Each subdirectory must supply rules for building sources it contributes
Athesar/Bsw/SoAd/%.o: ../Athesar/Bsw/SoAd/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"D:\Athena\Athena\Athesar\App" -I"D:\Athena\Athena\Athesar\Bsw\include" -I"D:\Athena\Athena\Athesar\Glue\vOs" -I"D:\Athena\Athena\Athesar\Bsw" -I"D:\Athena\Athena\Athesar\Cfg" -I"D:\Athena\Athena\Athesar\Glue" -I"D:\Athena\Athena\Athesar\Rte" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


