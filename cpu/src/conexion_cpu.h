/*
 * conexion_cpu.h
 *
 *  Created on: Sep 17, 2023
 *      Author: utnso
 */

#ifndef CONEXION_CPU_H_
#define CONEXION_CPU_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "instruccion.h"

extern t_log* logger;
extern int socket_memoria;
extern int socket_dispatch;
extern int socket_interrupt;
extern bool llego_interrupcion;
extern op_code codigo_interrupcion;


int iniciar_conexiones();

void esperar_dispatch(int socket);

void esperar_interrupt(int socket);



#endif /* CONEXION_CPU_H_ */
