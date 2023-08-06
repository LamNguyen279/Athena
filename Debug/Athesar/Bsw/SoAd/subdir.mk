################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Athesar/Bsw/SoAd/SoAd.c \
../Athesar/Bsw/SoAd/SoAd_Cfg.c \
../Athesar/Bsw/SoAd/SoAd_Priv.c 

OBJS += \
./Athesar/Bsw/SoAd/SoAd.o \
./Athesar/Bsw/SoAd/SoAd_Cfg.o \
./Athesar/Bsw/SoAd/SoAd_Priv.o 

C_DEPS += \
./Athesar/Bsw/SoAd/SoAd.d \
./Athesar/Bsw/SoAd/SoAd_Cfg.d \
./Athesar/Bsw/SoAd/SoAd_Priv.d 


# Each subdirectory must supply rules for building sources it contributes
Athesar/Bsw/SoAd/%.o: ../Athesar/Bsw/SoAd/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DSOAD_DEBUG -I"D:\Athena\Athena\Athesar\App" -I"D:\Athena\Athena\Athesar\Bsw\DoIP" -I"D:\Athena\Athena\Athesar\Bsw\include" -I"D:\Athena\Athena\Athesar\Glue\vOs" -I"D:\Athena\Athena\Athesar\Bsw" -I"D:\Athena\Athena\Athesar\Cfg" -I"D:\Athena\Athena\Athesar\Glue" -I"D:\Athena\Athena\Athesar\Rte" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


