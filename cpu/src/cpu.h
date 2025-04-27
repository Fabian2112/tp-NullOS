/*
 * cpu.h
 *
 *  Created on: Sep 14, 2023
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "conexion_cpu.h"
#include "instruccion.h"

t_log* logger;
t_config *config;
int socket_memoria;
int socket_dispatch;
int socket_interrupt;
bool llego_interrupcion = false;
op_code codigo_interrupcion;



#endif /* CPU_H_ */
