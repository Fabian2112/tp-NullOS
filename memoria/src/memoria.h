/*
 * memoria.h
 *
 *  Created on: Sep 15, 2023
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "conexion_memoria.h"
#include "memoria_instrucciones.h"
#include <semaphore.h>
#include "estructuras.h"

t_log* logger;
t_config *config;
int socket_memoria;
int socket_cpu ;
int socket_filesystem;
int socket_kernel;

// MEMORIA DE INSTRUCCIONES

char* instruccion_a_enviar;
t_list* lista_procesos;
sem_t enviar_instruccion;

// MEMORIA DE USUARIO

void* memoria_usuario;
t_list* procesos_en_memoria;
t_list* marcos;
t_list* lista_paginas_presentes;
int instante_carga;
int instante_acceso;

#endif /* MEMORIA_H_ */
