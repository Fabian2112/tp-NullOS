################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexion_cpu.c \
../src/cpu.c \
../src/datos_config.c \
../src/instruccion.c 

C_DEPS += \
./src/conexion_cpu.d \
./src/cpu.d \
./src/datos_config.d \
./src/instruccion.d 

OBJS += \
./src/conexion_cpu.o \
./src/cpu.o \
./src/datos_config.o \
./src/instruccion.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/tp-2023-2c-NullOS/shared/src -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/conexion_cpu.d ./src/conexion_cpu.o ./src/cpu.d ./src/cpu.o ./src/datos_config.d ./src/datos_config.o ./src/instruccion.d ./src/instruccion.o

.PHONY: clean-src

