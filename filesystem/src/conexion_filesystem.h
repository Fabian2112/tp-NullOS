/*
 * conexion_filesystem.h
 *
 *  Created on: Sep 23, 2023
 *      Author: utnso
 */

#ifndef CONEXION_FILESYSTEM_H_
#define CONEXION_FILESYSTEM_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "acciones.h"
#include "conexion_filesystem.h"
#include "swap.h"


extern t_log* logger;
extern int socket_filesystem;
extern int socket_memoria;
extern int socket_kernel;


extern pthread_t thread_memoria;
extern pthread_t thread_kernel;

void iniciar_conexiones();
int iniciar_servidor_fs();
void atender_kernel();
void procesar_peticion_kernel(int conexion);
void atender_memoria();

int conectar_con_servidor(char* ip, char* puerto, char* servidor);

#endif /* CONEXION_FILESYSTEM_H_ */
