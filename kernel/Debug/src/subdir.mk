################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexiones_kernel.c \
../src/consola.c \
../src/datos_config.c \
../src/kernel.c \
../src/planificacion.c \
../src/recursos.c 

C_DEPS += \
./src/conexiones_kernel.d \
./src/consola.d \
./src/datos_config.d \
./src/kernel.d \
./src/planificacion.d \
./src/recursos.d 

OBJS += \
./src/conexiones_kernel.o \
./src/consola.o \
./src/datos_config.o \
./src/kernel.o \
./src/planificacion.o \
./src/recursos.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/tp-2023-2c-NullOS/shared/src -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/conexiones_kernel.d ./src/conexiones_kernel.o ./src/consola.d ./src/consola.o ./src/datos_config.d ./src/datos_config.o ./src/kernel.d ./src/kernel.o ./src/planificacion.d ./src/planificacion.o ./src/recursos.d ./src/recursos.o

.PHONY: clean-src

