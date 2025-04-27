################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/acciones.c \
../src/conexion_filesystem.c \
../src/datos_config.c \
../src/estructuras.c \
../src/filesystem.c \
../src/swap.c 

C_DEPS += \
./src/acciones.d \
./src/conexion_filesystem.d \
./src/datos_config.d \
./src/estructuras.d \
./src/filesystem.d \
./src/swap.d 

OBJS += \
./src/acciones.o \
./src/conexion_filesystem.o \
./src/datos_config.o \
./src/estructuras.o \
./src/filesystem.o \
./src/swap.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/tp-2023-2c-NullOS/shared/src -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/acciones.d ./src/acciones.o ./src/conexion_filesystem.d ./src/conexion_filesystem.o ./src/datos_config.d ./src/datos_config.o ./src/estructuras.d ./src/estructuras.o ./src/filesystem.d ./src/filesystem.o ./src/swap.d ./src/swap.o

.PHONY: clean-src

