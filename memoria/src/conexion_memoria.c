/*
 * conexion_memoria.c
 *
 *  Created on: Sep 17, 2023
 *      Author: utnso
 */
#include "conexion_memoria.h"
pthread_t thread_cpu;
pthread_t thread_filesystem;
pthread_t thread_kernel;

int iniciar_conexiones(){

	socket_memoria = iniciar_servidor(puerto_escuha());

	if(socket_memoria < 0){
	        log_error(logger,"No se pudo crear el servidor");
	        return EXIT_FAILURE;
	}

	log_info(logger, "Servidor listo para recibir al cliente");


	while (1)
	    {
	        int socket_cliente = esperar_cliente(socket_memoria);
	        log_info(logger, "Tengo el cliente %d", socket_cliente);

	        int result = handshakeServidor(socket_cliente);
	        log_info(logger, "Se conectó:  %s", op_ModuloToString(result));

            switch(result){
            case CPU :
            	pthread_create(&thread_cpu, NULL,(void *)atenderCPU,(void *)socket_cliente);
            	pthread_detach(thread_cpu);
            	break;
            case FILESYSTEM:
            	pthread_create(&thread_filesystem, NULL, (void *)atenderFilesystem, (void *)socket_cliente);
            	pthread_detach(thread_filesystem);
            	break;
            case KERNEL:
	        	pthread_create(&thread_kernel,  NULL, (void *)atenderKernel, (void *)socket_cliente);
	        	pthread_detach(thread_kernel);
	        	break;

            default:
      	        break;
            }

	    }
	return EXIT_SUCCESS;
}


void atenderCPU(int socket_cliente){
   socket_cpu = socket_cliente;
   /*t_buffer* buffer;
   int desplazamiento = 0;
   int nro_pagina;
   int desplazamiento_pagina;
   int direccion_fisica;
   int valor; // por error de deserializar vacío
   int pid;*/

   	while (true)
	       {
	         switch (recibir_operacion(socket_cpu))
	         {
	         case _PEDIR_INSTRUCCION:
	            log_info(logger, "Se recibió peticion de instruccion \n");
	            //t_peticion_instruccion* buscar_instruccion;
	            //buscar_instruccion = deserializar_peticion_instruccion(socket_cpu);
	            t_list* pedir_instruccion_params = recibir_paquete(socket_cpu);
	           	int* pid_instruccion = list_get(pedir_instruccion_params, 0);
	          	int* program_counter = list_get(pedir_instruccion_params, 1);
	          	log_info(logger, "Buscar instrucción -> PID: < %d> - Program_Counter: < %d >" , *pid_instruccion, *program_counter);
	           // pid = buscar_instruccion->pid;
	            //int program_counter = buscar_instruccion->program_counter;

	            buscar_sgte_instruccion(*pid_instruccion, *program_counter);

	            list_destroy_and_destroy_elements(pedir_instruccion_params, (void*)free);
	            //free(buscar_instruccion);
	            break;

	         case _TAMANIO_PAGINA:
	            log_info(logger, "Se recibió peticion de tamanio de pagina \n");
	            t_list* tamanio_pagina_params = recibir_paquete(socket_cpu);
	            //int* valor = list_get(tamanio_pagina_params, 0);	// DE ADORNO

	            /*
	            buffer = crear_buffer();
	            desplazamiento = 0;
	            buffer-> stream = recibir_buffer(&(buffer -> size), socket_cpu);
	           	memcpy(&(valor), buffer->stream + desplazamiento, sizeof(int));*/
	           	//desplazamiento += sizeof(int);

	            char* tamanio_pagina = string_itoa(tam_pagina());
	            enviar_mensaje(tamanio_pagina, socket_cpu);
	            free(tamanio_pagina);

	            list_destroy_and_destroy_elements(tamanio_pagina_params, (void*)free);
	            //liberar_buffer(buffer);
	            break;

	         case _NRO_MARCO:
	        	 t_list* nro_marco_params = recibir_paquete(socket_cpu);
	        	 int* nro_pagina = list_get(nro_marco_params, 0);
	        	 int* pid_marco = list_get(nro_marco_params, 1);
	        	 /*buffer = crear_buffer();
	        	 desplazamiento = 0;
	        	 buffer-> stream = recibir_buffer(&(buffer -> size),socket_cpu);
	             memcpy(&(nro_pagina), buffer->stream + desplazamiento, sizeof(int));
	           	 desplazamiento += sizeof(int);
	           	 memcpy(&(pid), buffer->stream + desplazamiento, sizeof(int));*/
	           	 //desplazamiento += sizeof(int);

	           	 log_info(logger, "BUSCO NRO MARCO para PAGINA: %d y PID: %d", *nro_pagina, *pid_marco);
	           	 int nro_marco = buscar_marco(*nro_pagina, *pid_marco);
	           	 char* nro_marco_string = string_itoa(nro_marco);
	           	 log_info(logger, "SE OBTUVO EL MARCO NRO: %d", nro_marco);
	           	 enviar_mensaje(nro_marco_string, socket_cpu);
	           	 //liberar_buffer(buffer);
	           	 free(nro_marco_string);
	           	 list_destroy_and_destroy_elements(nro_marco_params, (void*)free);
	           	 break;

	         case _MOV_IN:
	        	 t_list* mov_in_params = recibir_paquete(socket_cpu);
	        	 int* dir_fisica_mov_in = list_get(mov_in_params, 0);
	        	 int* pid_mov_in = list_get(mov_in_params, 1);
	        	 int* tamanio_lectura = list_get(mov_in_params, 2);
	        	 uint32_t valor_lectura;
	        	 log_info(logger, "PID: %d - Accion: LEER - Direccion fisica: %d", *pid_mov_in, *dir_fisica_mov_in);

	        	 usleep(retardo_respuesta() * 1000);
	        	 memcpy(&valor_lectura, memoria_usuario + (*dir_fisica_mov_in), sizeof(uint32_t));
	        	 actualizar_ultimo_acceso(*pid_mov_in, *dir_fisica_mov_in);
	        	 log_info(logger, "VALOR LECTURA: %d", valor_lectura);

	        	 t_paquete* paquete= crear_paquete(MENSAJE);
	        	 agregar_a_paquete(paquete, &valor_lectura, sizeof(int));
	        	 enviar_paquete(paquete, socket_cpu);

	        	 list_destroy_and_destroy_elements(mov_in_params, (void*)free);
	             break;

	         case _MOV_OUT:
	        	 t_list* mov_out_params = recibir_paquete(socket_cpu);
	        	 int* dir_fisica_mov_out = list_get(mov_out_params, 0);
	        	 uint32_t* valor_escritura = list_get(mov_out_params, 1);
	        	 int* tamanio_escritura = list_get(mov_out_params, 2);
	        	 int* pid_mov_out = list_get(mov_out_params, 3);
	        	 log_info(logger, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d", *pid_mov_out, *dir_fisica_mov_out);
	        	 log_info(logger, "VALOR a ESRIBIR en memoria: %d", *valor_escritura);

	        	 usleep(retardo_respuesta() * 1000);
	        	 memcpy(memoria_usuario + (*dir_fisica_mov_out), valor_escritura, sizeof(uint32_t));
	        	 actualizar_ultimo_acceso(*pid_mov_out, *dir_fisica_mov_out);
	        	 actualizar_bit_modificado(*pid_mov_out, *dir_fisica_mov_out);

	        	 list_destroy_and_destroy_elements(mov_out_params, (void*)free);
	             break;

	   		 case -1:
	   			 log_info(logger, "Se desconectó CPU");
	   			 return;

	   		 default:
	   			 log_error(logger, "Respuesta de CPU desconocida");
	   			 continue;
	         }
	       }
}


void atenderKernel(int socket_cliente){
	  socket_kernel = socket_cliente;

		while (true){
			switch (recibir_operacion(socket_kernel)){
			case _INICIAR_PROCESO:
				//deserializar_iniciar_proceso(socket_kernel);
				log_info(logger, "INICIAR PROCESO");
				t_list* iniciar_proceso_params = recibir_paquete(socket_kernel);
				char* nombre_proceso = list_get(iniciar_proceso_params, 0);
				int* tamanio_proceso = list_get(iniciar_proceso_params, 1);
				int* pid_init  = list_get(iniciar_proceso_params, 2);
				log_info(logger,"Proceso nuevo : PID %d , nombre: %s , tamanio %d", *pid_init, nombre_proceso, *tamanio_proceso);

				crear_proceso(*pid_init, nombre_proceso, *tamanio_proceso);
				enviar_mensaje("OK", socket_kernel);

				list_destroy_and_destroy_elements(iniciar_proceso_params, (void*)free);
				break;

		    case _MEMORIA_INSTRUCCIONES:
				log_info(logger, "Se recibió un proceso nuevo \n");

				t_memoria_instruccion* proceso_nuevo;
				proceso_nuevo = deserializar_proceso_nuevo(socket_kernel);
				int pid = proceso_nuevo->pid;
				char* nombre_archivo = proceso_nuevo->nombre_archivo;
				log_info(logger, "Llegó el  -> PID: < %d> - Nombre de archivo: < %s >" , pid, nombre_archivo);

				agregar_proceso(pid, nombre_archivo);

				free(nombre_archivo);
				free(proceso_nuevo);
				break;

		    case _PAGE_FAULT:
		    	t_list* page_fault_params = recibir_paquete(socket_kernel);
		    	int* pid_page_fault = list_get(page_fault_params, 0);
		    	int* nro_pagina = list_get(page_fault_params, 1);

				log_info(logger, "PAGE FAULT: NRO Pagina: %d - PID: %d", *nro_pagina, *pid_page_fault);
				atender_page_fault(*pid_page_fault, *nro_pagina);
				log_info(logger, "SE CARGO LA PAGINA: %d del PID: %d", *nro_pagina, *pid_page_fault);

				char* pid_pf = string_itoa(*pid_page_fault);
				enviar_mensaje(pid_pf, socket_kernel);
				free(pid_pf);
				//liberar_buffer(buffer);
				list_destroy_and_destroy_elements(page_fault_params, (void*)free);
				break;


		    case _FINALIZAR_PROCESO:
		    	t_list* finalizar_proceso_params = recibir_paquete(socket_kernel);
		    	int* pid_end = list_get(finalizar_proceso_params, 0);
		    	log_info(logger,"Se recibió petición de finalizar estructuras del PID:  %d ", *pid_end);

		    	/*buffer = crear_buffer();
		    	desplazamiento = 0;
		    	buffer-> stream = recibir_buffer(&(buffer -> size), socket_kernel);
		   		memcpy(&(pid), buffer->stream + desplazamiento, sizeof(int));*/

				eliminar_instrucciones_proceso(*pid_end);

				t_proceso_memoria* proceso = buscar_proceso(*pid_end);
				if(proceso == NULL){
					log_error(logger, "Las estructuras de memoria del PID: %d NO existen", *pid_end);
					abort();
				}
				destruir_proceso(proceso);
				log_info(logger,"Se eliminaron correctamente las estructuras de memoria del PID:  %d ", *pid_end);
				list_destroy_and_destroy_elements(finalizar_proceso_params, (void*)free);
				//liberar_buffer(buffer);
				break;


		   	case -1:
				log_info(logger, "Se desconectó KERNEL Dispatch");
				return;

		   	default:
		   		log_error(logger, "Respuesta de Kernel dispatch desconocida");
		   		continue;
			}
		}
}


void atenderFilesystem(int socket_cliente){
	socket_filesystem = socket_cliente;

	  while (true){
		  switch (recibir_operacion(socket_filesystem)){
		  case _READ:
			  t_list* f_read_params = recibir_paquete(socket_filesystem);
			  int* dir_fisica_f_read = list_get(f_read_params, 0);
			  //char* valor_escritura = list_get(f_read_params, 1);
			  void* valor_escritura; //= malloc(tam_pagina());
			  valor_escritura = list_get(f_read_params, 1);
			  int* tamanio_escritura = list_get(f_read_params, 2);
			  int* pid_f_read = list_get(f_read_params, 3);
			  log_info(logger, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Valor: %s", *pid_f_read, *dir_fisica_f_read, (char*)valor_escritura);

			  usleep(retardo_respuesta() * 1000);
			  memcpy(memoria_usuario + *dir_fisica_f_read, valor_escritura, *tamanio_escritura);
			  actualizar_ultimo_acceso(*pid_f_read, *dir_fisica_f_read);
			  actualizar_bit_modificado(*pid_f_read, *dir_fisica_f_read);

			  log_info(logger, "VALOR ESCRITO en memoria CORRECTAMENTE");

		  	  enviar_mensaje("OK", socket_filesystem);

		  	  list_destroy_and_destroy_elements(f_read_params, (void*)free);
		  	  //free(valor_escritura);
		  	  break;

		  case _WRITE:
			  t_list* f_write_params = recibir_paquete(socket_filesystem);
			  int* dir_fisica_f_write = list_get(f_write_params, 0);
			  int* pid_f_write = list_get(f_write_params, 1);
			  int* tamanio_lectura = list_get(f_write_params, 2);
			  void* valor_lectura = malloc(*tamanio_lectura);
			  log_info(logger, "PID: %d - Accion: LEER - Direccion fisica: %d", *pid_f_write, *dir_fisica_f_write);

			  usleep(retardo_respuesta() * 1000);
			  memcpy(valor_lectura, memoria_usuario + *dir_fisica_f_write, *tamanio_lectura);
			  actualizar_ultimo_acceso(*pid_f_write, *dir_fisica_f_write);

			  //char* mensaje = malloc((*tamanio_lectura) + 1);
			  //mensaje = strcat((char*)valor_lectura, "/0");
			  //enviar_mensaje(valor_lectura, socket_filesystem);
			  t_paquete* paquete = crear_paquete(_WRITE);
			  agregar_a_paquete(paquete, valor_lectura, *tamanio_lectura);
			  enviar_paquete(paquete, socket_filesystem);

	     	  free(valor_lectura);
			  list_destroy_and_destroy_elements(f_write_params, (void*)free);
		  	  break;

	  	 case -1:
			 log_info(logger, "Se desconectó FILESYSTEM");
		  	 return;

		 default:
		  	log_error(logger, "Mensaje del filesystem desconocido");
		  	continue;
		 }
	  }
}


void log_valor_espacio_usuario(char* valor, int tamanio){
	char* valor_log = malloc(tamanio);
	memcpy(valor_log, valor, tamanio);
	memcpy(valor_log + tamanio, "\0", 1);
	int tamanio_valor = strlen(valor_log);
	log_info(logger, "LECTURA/ESCRITURA EN MEMORIA: VALOR: %s - TAMAÑO: %d", valor_log, tamanio_valor);
}



void deserializar_iniciar_proceso(int socket){
	int tamanio_nombre, tamanio_proceso, pid;
    int desplazamiento = 0;
    t_buffer* buffer = crear_buffer();
	 buffer-> stream = recibir_buffer(&(buffer -> size), socket); //printf("Se recibió %d bytes", buffer->size);
	 	 memcpy(&(tamanio_nombre), buffer->stream + desplazamiento, sizeof(int));

	 	log_info(logger,"TAMANIO NOMBRE: %d", tamanio_nombre);

	 	 desplazamiento += sizeof(int);
	 	 char* nombre_proceso = malloc(tamanio_nombre);
	 	 memcpy(nombre_proceso, buffer->stream + desplazamiento, tamanio_nombre);
	 	 desplazamiento += sizeof(tamanio_nombre);
	 	 memcpy(&(tamanio_proceso), buffer->stream + desplazamiento, sizeof(int));
	 	 desplazamiento += sizeof(int);
	 	 memcpy(&(pid), buffer->stream + desplazamiento, sizeof(int));


	liberar_buffer(buffer);

	log_info(logger,"Proceso nuevo : PID %d , nombre: %s , tamanio %d", pid, nombre_proceso, tamanio_proceso);

	crear_proceso(pid, nombre_proceso, tamanio_proceso);

	free(nombre_proceso);
}


