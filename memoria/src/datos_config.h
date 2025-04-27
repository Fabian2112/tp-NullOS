/*
 * datos_config.h
 *
 *  Created on: Sep 16, 2023
 *      Author: utnso
 */

#ifndef DATOS_CONFIG_H_
#define DATOS_CONFIG_H_


#include <shared_utils.h>
#include "memoria_instrucciones.h"

extern t_config* config;

char* puerto_escuha();
char* ip_filesystem();
char* puerto_filesystem();
int   tam_memoria();
int   tam_pagina();
char* path_instrucciones();
int   retardo_respuesta();
char* algoritmo_reemplazo();

#endif /* DATOS_CONFIG_H_ */
