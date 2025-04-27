/*
 * filesystem.h
 *
 *  Created on: Sep 15, 2023
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "conexion_filesystem.h"
#include "estructuras.h"
#include "acciones.h"
#include "swap.h"

//ESTRUCTURAS FILESYSTEM
t_list* lista_fcbs;
void* bloques_datos ;
uint32_t entrada_fat;

uint32_t* tablaFat;   //archivo fat

t_list* lista_swaps;

int file_dat;
int file_fat;

t_log* logger;
t_config *config;


pthread_t thread_memoria;
pthread_t thread_kernel;


// VARIABLES DE CONEXION
int socket_filesystem; // SOCKET SERVIDOR DE FILESYSTEM AL CUAL SE CONECTA EL MODULO KERNEL
// SOCKET_CLIENTE (kernel) LO MANEJO COMO VARIABLE LOCAL EN atender_kernel();
int socket_memoria;
int socket_kernel;


void liberarEstructuras();


#endif /* FILESYSTEM_H_ */
