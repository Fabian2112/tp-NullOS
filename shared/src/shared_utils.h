/*
 * shared_utils.h
 *
 *  Created on: Sep 14, 2023
 *      Author: utnso
 */

#ifndef SHARED_UTILS_H_
#define SHARED_UTILS_H_

#include "conexion.h"
#include "config_shared.h"
#include "log_shared.h"
#include "proceso.h"
#include "TypeDef.h"

char *mi_funcion_compartida();

enum op_modulo
{
    CPU ,
    FILESYSTEM,
    KERNEL,
	MEMORIA
} ;

typedef enum {
	FIFO,
	RR,
	PRIORIDADES
} code_algoritmo;


typedef struct {
	t_list* cola;
	pthread_mutex_t mutex_cola;
} t_monitor;


// handshake
int handshakeCliente(int socket_cliente, int modulo);
int handshakeServidor(int socket_cliente);

const char *op_ModuloToString(int opCode);

t_monitor* monitor_create();
void monitor_destroy(t_monitor* monitor);

#endif /* SHARED_UTILS_H_ */
