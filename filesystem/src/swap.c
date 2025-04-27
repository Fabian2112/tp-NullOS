/*
 * swap.c
 *
 *  Created on: Dec 9, 2023
 *      Author: utnso
 */
#include "swap.h"


/* 	===============================================================================================================
				Asignar  y finalizar lista de bloques SWAP
	===============================================================================================================*/
 void peticion_init_swap(int tamanio_pid, int socket_cliente){
	 int cantidad_bloques_swap = ceil( tamanio_pid/tamanio_bloque() );

	 t_list* lista_bloques_asignados = list_create();

	 while(cantidad_bloques_swap > 0 ){
        int index ; //= malloc(sizeof(int));
		index= siguiente_bloque_swap_libre(); //log_info(logger, "SWAP LIBRE: %d", index);
		 inicializar_bloque_swap(index);
		 list_add(lista_bloques_asignados, index);
		// log_info(logger, "Se agregó el bloque swap: %d ", index);
		 cantidad_bloques_swap--;

       }

	 t_list_iterator* iterador_lst = list_iterator_create(lista_bloques_asignados);

		  			     	   int indice = iterador_lst->index;

		  			     	    while(list_iterator_has_next(iterador_lst)){
		  			     	       int bloque_asignado = list_iterator_next(iterador_lst);
		  			     	        indice = iterador_lst->index;
		  			     	       log_info(logger,"Se asigna a la solicitud SWAP: nro_bloque %d", bloque_asignado);

		  			     	    }
		  			     	    list_iterator_destroy(iterador_lst);


    enviar_lista_swap(lista_bloques_asignados, socket_cliente);



 }

 void enviar_lista_swap(t_list* lista_asignados, int socket_cliente){
		 int cantidad_bloques = list_size(lista_asignados);

	 t_paquete* paquete = crear_paquete(MENSAJE);
	 agregar_a_paquete(paquete, &cantidad_bloques, sizeof(int));
	 for (int i = 0; i < cantidad_bloques; i++)
	   {
	   int nro = list_get(lista_asignados, i);
	     agregar_a_paquete(paquete, &(nro), sizeof(int)); //log_info(logger,"serializo valor %d", nro);
	   }
	 enviar_paquete(paquete,socket_cliente);

	 list_destroy(lista_asignados);


 }

void peticion_fin_swap( t_list* bloques_swap){
   int cant_bloques = list_size(bloques_swap);

	for (int i = 0 ; i < cant_bloques ; i++){
		int nro = list_get(bloques_swap, i);
		liberar_bloque_swap(nro);
	}

	usleep(retardo_acceso_bloque() * 1000);

	 list_destroy(bloques_swap);

 }

/* 	===============================================================================================================
				 SWAP IN    SWAP OUT
	===============================================================================================================*/

void swap_out(void* valor_escritura, uint32_t nro_bloque){  //Asigno valor en bloque swap

	escribir_bloque_swap(valor_escritura, nro_bloque );

   usleep(retardo_acceso_bloque() * 1000);

}


void swap_in(uint32_t nro_bloque, int socket_cliente){ //Recibo nro de bloque y devuelvo el contenido

   //obtengo el valor del bloque y lo envío a memoria
	 void* valor_swap_in; // = malloc(tamanio_bloque());
	 valor_swap_in = leer_bloque_swap(nro_bloque);
	 log_info(logger, "Acceso SWAP IN : %d", nro_bloque );


	 usleep(retardo_acceso_bloque() * 1000);

	 enviar_valor_swap_in(valor_swap_in,socket_cliente);

 }

void enviar_valor_swap_in(void* valor, int socket_cliente){
	  t_paquete* paquete = crear_paquete(MENSAJE);
	    agregar_a_paquete(paquete, valor, tamanio_bloque() );  //log_info(logger, "llego1");
		enviar_paquete(paquete, socket_cliente); //log_info(logger, "llego2");

		free(valor);
}

