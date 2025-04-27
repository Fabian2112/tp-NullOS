/*
 * datos_config.c
 *
 *  Created on: Sep 23, 2023
 *      Author: utnso
 */

#include "datos_config.h"

char* ip_memoria(){
	return config_get_string_value(config, "IP_MEMORIA");
}

char* puerto_memoria(){
	return config_get_string_value(config, "PUERTO_MEMORIA");
}

char* puerto_escucha(){
	return config_get_string_value(config, "PUERTO_ESCUCHA");
}

char* path_fat(){
	return config_get_string_value(config, "PATH_FAT");
}

char* path_bloques(){
	return config_get_string_value(config, "PATH_BLOQUES");
}

char* path_fcb(){
	return config_get_string_value(config, "PATH_FCB");
}

int cant_bloques_total(){
	return config_get_int_value(config, "CANT_BLOQUES_TOTAL");
}

int cant_bloques_swap(){
	return config_get_int_value(config, "CANT_BLOQUES_SWAP");
}

int tamanio_bloque(){
	return config_get_int_value(config, "TAM_BLOQUE");
}

int retardo_acceso_bloque(){
	return config_get_int_value(config, "RETARDO_ACCESO_BLOQUE");
}

int retardo_acceso_fat(){
	return config_get_int_value(config, "RETARDO_ACCESO_FAT");
}

