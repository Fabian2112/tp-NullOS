/*
 * datos_config.h
 *
 *  Created on: Sep 23, 2023
 *      Author: utnso
 */

#ifndef DATOS_CONFIG_H_
#define DATOS_CONFIG_H_

#include <shared_utils.h>

extern t_config* config;

char* ip_memoria();
char* puerto_memoria();
char* puerto_escucha();
char* path_fat();
char* path_bloques();
char* path_fcb();
int cant_bloques_total();
int cant_bloques_swap();
int tamanio_bloque();
int retardo_acceso_bloque();
int retardo_acceso_fat();


#endif /* DATOS_CONFIG_H_ */
