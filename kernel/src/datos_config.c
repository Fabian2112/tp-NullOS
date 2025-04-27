/*
 * datos_config.c
 *
 *  Created on: Sep 22, 2023
 *      Author: utnso
 */

#include "../include/datos_config.h"

char* ip_memoria(){
	return config_get_string_value(config, "IP_MEMORIA");
}

char* puerto_memoria(){
	return config_get_string_value(config, "PUERTO_MEMORIA");
}

char* ip_filesystem(){
	return config_get_string_value(config, "IP_FILESYSTEM");
}

char* puerto_filesystem(){
	return config_get_string_value(config, "PUERTO_FILESYSTEM");
}

char* ip_cpu(){
	return config_get_string_value(config, "IP_CPU");
}

char* puerto_dispatch(){
	return config_get_string_value(config, "PUERTO_CPU_DISPATCH");
}

char* puerto_interrupt(){
	return config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
}

char* algoritmo_planificacion(){
	return config_get_string_value(config, "ALGORITMO_PLANIFICACION");
}

int quantum(){
	return config_get_int_value(config, "QUANTUM");
}

char** recursos(){
	return config_get_array_value(config, "RECURSOS");
}

char** instancias_recursos(){
	return config_get_array_value(config, "INSTANCIAS_RECURSOS");
}

int grado_multiprogramacion(){
	return config_get_int_value(config, "GRADO_MULTIPROGRAMACION_INI");
}
