################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexion.c \
../src/config_shared.c \
../src/log_shared.c \
../src/proceso.c \
../src/shared_utils.c 

C_DEPS += \
./src/conexion.d \
./src/config_shared.d \
./src/log_shared.d \
./src/proceso.d \
./src/shared_utils.d 

OBJS += \
./src/conexion.o \
./src/config_shared.o \
./src/log_shared.o \
./src/proceso.o \
./src/shared_utils.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/conexion.d ./src/conexion.o ./src/config_shared.d ./src/config_shared.o ./src/log_shared.d ./src/log_shared.o ./src/proceso.d ./src/proceso.o ./src/shared_utils.d ./src/shared_utils.o

.PHONY: clean-src

