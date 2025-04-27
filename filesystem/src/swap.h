/*
 * swap.h
 *
 *  Created on: Dec 9, 2023
 *      Author: utnso
 */

#ifndef SWAP_H_
#define SWAP_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "estructuras.h"
#include "conexion_filesystem.h"


extern t_list* lista_swaps;


//iniciar y finalizar proceso
 void peticion_init_swap(int tamanio_pid, int socket_cliente);
 void enviar_lista_swap(t_list* lista_asignados, int socket_cliente);
 void peticion_fin_swap( t_list* bloques_swap);

 //SWAP IN Y SWAP OUT
 void swap_out(void* valor, uint32_t nro_bloque);

 void swap_in(uint32_t nro_bloque, int socket_cliente);
 void enviar_valor_swap_in(void* valor, int socket_cliente);



#endif /* SWAP_H_ */
