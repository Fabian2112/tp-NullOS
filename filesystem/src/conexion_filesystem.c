/*
 * conexion_filesystem.c
 *
 *  Created on: Sep 23, 2023
 *      Author: utnso
 */

#include "conexion_filesystem.h"

void iniciar_conexiones(){

	// LEVANTO SERVIDOR
		socket_filesystem = iniciar_servidor_fs(puerto_escucha());


		while (5)
			    {
			        int socket_cliente = esperar_cliente(socket_filesystem);
			        log_info(logger, "Tengo el cliente %d", socket_filesystem);

			        int result = handshakeServidor(socket_cliente);
			        log_info(logger, "Se conectó:  %s", op_ModuloToString(result));


		            switch(result){
		            case KERNEL :
		            	pthread_create(&thread_kernel, NULL,(void *)atender_kernel,(void *)socket_cliente);
		            	pthread_detach(thread_kernel);
		            	break;
		            case MEMORIA:
		            	pthread_create(&thread_memoria, NULL, (void *)atender_memoria, (void *)socket_cliente);
		            	pthread_detach(thread_memoria);
		            	break;
		            default:
		      	        break;
		            }
			    }
}

int iniciar_servidor_fs(char* puerto){

	int socket = iniciar_servidor(puerto);
	if(socket < 0){
	        log_error(logger,"No se pudo crear el servidor");
	        return EXIT_FAILURE;
	    }
	log_info(logger, "Servidor listo para recibir cliente");

	return socket;
}

void atender_kernel(int socket_cliente){
//	socket_kernel = socket_cliente;
	int desplazamiento= 0;
	 int tamanio_nombre= 0;
	 t_buffer* buffer;

		while(true){
			switch(recibir_operacion(socket_cliente)){	// RECIBIR OPERACION CIERRA EL SOCKET ANTE UN ERROR	Y RETORNA -1

			case _OPEN:
				 /*desplazamiento = 0;
				 buffer = crear_buffer();
				 buffer-> stream = recibir_buffer(&(buffer -> size), socket_cliente);
				 memcpy(&tamanio_nombre, buffer->stream + desplazamiento, sizeof(int));
 			 	 desplazamiento += sizeof(int);
   				char* nombre_archivo_open = malloc(tamanio_nombre);
				 memcpy(nombre_archivo_open, buffer->stream + desplazamiento, tamanio_nombre);
				 liberar_buffer(buffer);*/
				t_list* open_params = recibir_paquete(socket_cliente);
				char* nombre_archivo_open = list_get(open_params, 0);
				abrir_archivo(nombre_archivo_open);
				log_info(logger,"Abrir archivo %s", nombre_archivo_open);

				//free(nombre_archivo_open); NO HACER FREE!!
				liberar_conexion(socket_cliente);
				break;

			case _TRUNCATE:
				 int tamanio_truncar ;
				 desplazamiento = 0;
				 buffer = crear_buffer();

				 buffer-> stream = recibir_buffer(&(buffer -> size), socket_cliente);
				 memcpy(&tamanio_truncar, buffer->stream + desplazamiento, sizeof(int));
 			 	 desplazamiento += sizeof(int);
 			 	 memcpy(&tamanio_nombre, buffer->stream + desplazamiento, sizeof(int));
			     desplazamiento += sizeof(int);
				char* nombre_archivo_truncar = malloc(tamanio_nombre);
				 memcpy(nombre_archivo_truncar, buffer->stream + desplazamiento, tamanio_nombre);
				 liberar_buffer(buffer);

				 log_info(logger,"Truncar Archivo : %s, tamanio = %d", nombre_archivo_truncar, tamanio_truncar);

				 truncar_archivo(nombre_archivo_truncar, tamanio_truncar);

                //Enviar ok para desbloquear al pid
				 enviar_mensaje("OK", socket_cliente);

				 liberar_conexion(socket_cliente);
				 free(nombre_archivo_truncar);


			break;

			case _WRITE:

			t_list* write_in_params = recibir_paquete(socket_cliente);
				char* nombre_archivo_escritura = list_get(write_in_params, 0);
				int* puntero_seek = list_get(write_in_params, 1);
				int* direccion_fisica = list_get(write_in_params, 2);
				int* pid_escritura= list_get(write_in_params, 3);


				log_info(logger,"Escribir archivo: %s, puntero: %d , DF: %d , PID: %d", nombre_archivo_escritura, *puntero_seek,*direccion_fisica, *pid_escritura);

				 peticion_escritura(nombre_archivo_escritura, *puntero_seek, *direccion_fisica, *pid_escritura);

                //Enviar ok para desbloquear al pid
				 enviar_mensaje("OK", socket_cliente);

				 liberar_conexion(socket_cliente);
				 list_destroy_and_destroy_elements(write_in_params, (void*)free);
				 //free(nombre_archivo_escritura);

			break;
			case _READ:
    			t_list* read_in_params = recibir_paquete(socket_cliente);
				char* nombre_archivo_lectura = list_get(read_in_params, 0);
				int* puntero_seek_r = list_get(read_in_params, 1);
				int* direccion_fisica_r = list_get(read_in_params, 2);
				int* pid_lectura= list_get(read_in_params, 3);


				log_info(logger,"Leer archivo: %s, puntero: %d , DF: %d , PID: %d", nombre_archivo_lectura, *puntero_seek_r,*direccion_fisica_r, *pid_lectura);

				 peticion_lectura(nombre_archivo_lectura, *puntero_seek_r, *direccion_fisica_r, *pid_lectura);

                //Enviar ok para desbloquear al pid
				 enviar_mensaje("OK", socket_cliente);

				 liberar_conexion(socket_cliente);
				// free(nombre_archivo_lectura);
				 list_destroy_and_destroy_elements(read_in_params, (void*)free);
      		break;

			case -1:
				log_info(logger, "Se desconectó KERNEL");
	     		return;

		   default:
			log_error(logger, "Mensaje de Kernel desconocida");
			continue;
			}
		}

}


void atender_memoria(int socket_cliente){
	int desplazamiento= 0;
	int tamanio= 0;
	t_buffer* buffer;

    // int socket_memoria = socket_cliente;

		while(true){
			switch(recibir_operacion(socket_cliente)){	// RECIBIR OPERACION CIERRA EL SOCKET ANTE UN ERROR	Y RETORNA -1

			case MENSAJE:
				recibir_mensaje(socket_cliente);

				break;
		    case _INICIAR_PROCESO:
		    	t_list* init_in_params = recibir_paquete(socket_cliente);
		    	    int* tamanio_pid = list_get(init_in_params, 0);
		    		log_info(logger,"Se solicita asignación de SWAP para PID de tamanio %d",*tamanio_pid);

		    		 peticion_init_swap( *tamanio_pid, socket_cliente);

		    	     	 liberar_conexion(socket_cliente);
		    		     list_destroy_and_destroy_elements(init_in_params, (void*)free);
		      break;

		    case _FINALIZAR_PROCESO:
		    	t_list* bloques_SWAP = list_create();
		    	t_list* fin_params = recibir_paquete(socket_cliente); //log_info(logger, "llegó");

		    	int* cant_bloques = list_get(fin_params, 0); //log_info(logger, "llegó");
		    	for (int i = 1; i <= *cant_bloques ; i++){
		    		int *nro_bloque_swap = list_get(fin_params, i);  //log_info(logger,"valor i %d, bloque %d", i, *nro_bloque_swap );
		    		list_add(bloques_SWAP, *nro_bloque_swap);
		    	}

		    	peticion_fin_swap(bloques_SWAP);

		    	log_info(logger, "Se liberaron los bloques swap solicitados");

		   	  /*  t_list_iterator* iterador_lst = list_iterator_create(lista_swaps);
		   	    t_swap_block* carga = NULL;//malloc(sizeof(t_swap_block));
		   		  			     	    while(list_iterator_has_next(iterador_lst)){
		   		  			     	 carga = list_iterator_next(iterador_lst);
		   		  			     	        log_info(logger,"Nro_Bloque %d,  ocupado?%d", carga->nro_bloque , carga->ocupado);

		   		  			     	    }
		   		  			     	    list_iterator_destroy(iterador_lst);
		   		  			     	    */

		    	     	 liberar_conexion(socket_cliente);
		    		     list_destroy_and_destroy_elements(fin_params, (void*)free);
		      break;


		     case _SWAP_OUT:

		    	 void* valor_recibido; // = malloc(tamanio_bloque());
		    	t_list* swap_out_params = recibir_paquete(socket_cliente);
		    	   valor_recibido = list_get(swap_out_params, 0);
			      int* nro_bloque_out = list_get(swap_out_params, 1);
		    		log_info(logger,"Petición SWAP OUT en bloque %d",*nro_bloque_out);

		    		swap_out(valor_recibido, *nro_bloque_out);
		    		 log_info(logger, "Fin de solicitud de SWAP OUT del bloque %d", *nro_bloque_out);

		    		 //free(valor_recibido);
		    		  liberar_conexion(socket_cliente);
		    		  //list_destroy_and_destroy_elements(swap_out_params, (void*)free);
		      break;

		     case _SWAP_IN:
		    	t_list* swap_in_params = recibir_paquete(socket_cliente);
		    	    int* nro_bloque_in = list_get(swap_in_params, 0);
		    		log_info(logger,"Petición SWAP IN del bloque %d",*nro_bloque_in);

		    		swap_in(*nro_bloque_in, socket_cliente);

		    	     	 liberar_conexion(socket_cliente);
		    		     list_destroy_and_destroy_elements(swap_in_params, (void*)free);
		      break;

		     case -1:
				log_error(logger, "Memoria se desconecto del socket %d", socket_cliente);
				return;
			default:
				log_error(logger, "La operacion solicitada por Memoria  es desconocida");
				continue;
			}
		}
}

int conectar_con_servidor(char* ip, char* puerto, char* servidor){
	log_info(logger, "Conectando con servidor %s en IP: %s y Puerto: %s", servidor, ip, puerto);

	int socket = crear_conexion(ip, puerto);
	if (socket < 0){
		log_error(logger,"No se pudo conectar con el servidor %s", servidor);
		return EXIT_FAILURE;
	}
	log_info(logger, "El socket conectado es: %d", socket);

	int result = handshakeCliente(socket, FILESYSTEM);
	if(result == 0)
		log_info(logger, "Handshake con %s exitoso", servidor);
	else{
		log_error(logger, "Handshake con %s fallido", servidor);
		return EXIT_FAILURE;
	}

	return socket;
	}


