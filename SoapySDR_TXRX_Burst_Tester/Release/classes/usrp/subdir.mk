################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../classes/usrp/USRP_Device.cpp 

OBJS += \
./classes/usrp/USRP_Device.o 

CPP_DEPS += \
./classes/usrp/USRP_Device.d 


# Each subdirectory must supply rules for building sources it contributes
classes/usrp/%.o: ../classes/usrp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPETIMENTAL_CXX0X__ -D__cplusplus=201103L -O3 -Wall -c -fmessage-length=0 -std=c++0x -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


