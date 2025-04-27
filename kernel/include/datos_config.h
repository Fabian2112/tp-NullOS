/*
 * datos_config.h
 *
 *  Created on: Sep 22, 2023
 *      Author: utnso
 */

#ifndef DATOS_CONFIG_H_
#define DATOS_CONFIG_H_

#include <shared_utils.h>

extern t_config* config;

char* ip_memoria();
char* puerto_memoria();
char* ip_filesystem();
char* puerto_filesystem();
char* ip_cpu();
char* puerto_dispatch();
char* puerto_interrupt();
char* algoritmo_planificacion();
int   quantum();
char** recursos();
char** instancias_recursos();
int grado_multiprogramacion();



#endif /* DATOS_CONFIG_H_ */
