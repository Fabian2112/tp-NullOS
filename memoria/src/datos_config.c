/*
 * datos_config.c
 *
 *  Created on: Sep 16, 2023
 *      Author: utnso
 */

#include "datos_config.h"

char* puerto_escuha(){
	return config_get_string_value(config, "PUERTO_ESCUCHA");
}

char* ip_filesystem(){
	return config_get_string_value(config, "IP_FILESYSTEM");
}

char* puerto_filesystem(){
	return config_get_string_value(config, "PUERTO_FILESYSTEM");
}

int  tam_memoria(){
	return config_get_int_value(config, "TAM_MEMORIA");
}

int  tam_pagina(){
	return config_get_int_value(config, "TAM_PAGINA");
}

char* path_instrucciones(){
	return config_get_string_value(config, "PATH_INSTRUCCIONES");
}

int  retardo_respuesta(){
	return config_get_int_value(config, "RETARDO_RESPUESTA");
}

char* algoritmo_reemplazo(){
	return config_get_string_value(config, "ALGORITMO_REEMPLAZO");
}

