/*
 * conexion_memoria.h
 *
 *  Created on: Sep 17, 2023
 *      Author: utnso
 */

#ifndef CONEXION_MEMORIA_H_
#define CONEXION_MEMORIA_H_

#include <shared_utils.h>
#include "memoria_instrucciones.h"
#include "estructuras.h"

extern t_log* logger;
extern int socket_memoria;
extern int socket_cpu;
extern int socket_filesystem;
extern int socket_kernel;
extern char* instruccion_a_enviar;

extern void* memoria_usuario;

int iniciar_conexiones();


void atenderCPU(int socket_cliente);
void atenderKernel(int socket_cliente);
void atenderFilesystem(int socket_cliente);

void log_valor_espacio_usuario(char* valor, int tamanio);
void deserializar_iniciar_proceso(int socket);

#endif /* CONEXION_MEMORIA_H_ */
