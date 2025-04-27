/*
 * datos_config.c
 *
 *  Created on: Sep 16, 2023
 *      Author: utnso
 */

#include "datos_config.h"


char* ip_memoria(){
	return config_get_string_value(config, "IP_MEMORIA");
}

char* puerto_memoria(){
	return config_get_string_value(config, "PUERTO_MEMORIA");
}

char* puerto_dispatch(){
	return config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
}

char* puerto_interrupt(){
	return config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
}
