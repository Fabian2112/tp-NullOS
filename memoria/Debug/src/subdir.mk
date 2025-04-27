################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexion_memoria.c \
../src/datos_config.c \
../src/estructuras.c \
../src/memoria.c \
../src/memoria_instrucciones.c 

C_DEPS += \
./src/conexion_memoria.d \
./src/datos_config.d \
./src/estructuras.d \
./src/memoria.d \
./src/memoria_instrucciones.d 

OBJS += \
./src/conexion_memoria.o \
./src/datos_config.o \
./src/estructuras.o \
./src/memoria.o \
./src/memoria_instrucciones.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/tp-2023-2c-NullOS/shared/src -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/conexion_memoria.d ./src/conexion_memoria.o ./src/datos_config.d ./src/datos_config.o ./src/estructuras.d ./src/estructuras.o ./src/memoria.d ./src/memoria.o ./src/memoria_instrucciones.d ./src/memoria_instrucciones.o

.PHONY: clean-src

