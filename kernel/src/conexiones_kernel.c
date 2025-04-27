/*
 * conexiones_kernel.c
 *
 *  Created on: Sep 22, 2023
 *      Author: utnso
 */

#include "../include/conexiones_kernel.h"
#include "../include/datos_config.h"


bool iniciar_conexiones(){

	socket_memoria = conectar_con_servidor(ip_memoria(), puerto_memoria(), "MEMORIA");

	//socket_filesystem = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM"); //despues de implementar Filesystem esta conexiónn será a demanda

    socket_dispatch = conectar_con_servidor(ip_cpu(), puerto_dispatch(), "CPU_DISPATCH");

	socket_interrupt = conectar_con_servidor(ip_cpu(), puerto_interrupt(), "CPU_INTERRUPT");


	return socket_memoria != EXIT_FAILURE
			//&& socket_filesystem != EXIT_FAILURE
			&& socket_dispatch != EXIT_FAILURE
			&& socket_interrupt != EXIT_FAILURE;
}

void liberar_conexiones(){
	liberar_conexion(socket_memoria);
	//liberar_conexion(socket_filesystem);
	liberar_conexion(socket_dispatch);
	liberar_conexion(socket_interrupt);
}

int conectar_con_servidor(char* ip, char* puerto, char* servidor){
	log_info(logger, "Conectando con servidor %s en IP: %s y Puerto: %s", servidor, ip, puerto);

	int socket = crear_conexion(ip, puerto);
	if (socket < 0){
		log_error(logger,"No se pudo conectar con el servidor %s", servidor);
		return EXIT_FAILURE;
	}
	log_info(logger, "El socket conectado es: %d", socket);

	int result = handshakeCliente(socket, KERNEL);
	if(result == 0)
		log_info(logger, "Handshake con %s exitoso", servidor);
	else{
		log_error(logger, "Handshake con %s fallido", servidor);
		return EXIT_FAILURE;
	}

	//enviar_mensaje("HOLA, SOY KERNEL \n", socket); //PARA PROBAR

	return socket;	// COMO YA MANEJO LOS ERRORES SIEMPRE RETORNA CORRECTAMENTE EL FILE DESCRIPTOR, NO RETORNA -1
}

void crear_hilos_de_conexion(){
	pthread_create(&conexion_memoria, NULL, (void *)mensaje_memoria, &socket_memoria);
	pthread_create(&conexion_cpu_dispatch, NULL, (void *)recibir_cpu_dispatch, &socket_dispatch);
	/*pthread_create(&conexion_filesystem, NULL, (void *)recibir_filesystem, &socket_filesystem);
	pthread_create(&conexion_cpu_interrupt, NULL, (void *)enviar_cpu_interrupt, &socket_interrupt);*/
}


void esperar_hilos_de_conexion(){
	pthread_join(conexion_memoria, NULL);
	pthread_detach(conexion_cpu_dispatch);
	/*pthread_join(conexion_filesystem, NULL);
	pthread_join(conexion_cpu_interrupt, NULL);*/
}

void mensaje_memoria(int socket_memoria){
  //implementar los mensajes a memoria

}


void recibir_filesystem(int socket_filesystem){
	int conexion = socket_filesystem;

	while(true){ // Chequeo socket_filesystem != -1?
		switch(recibir_operacion(conexion)){

		case MENSAJE:
			recibir_mensaje(conexion);
			//close(conexion);
			break;
		case PAQUETE:
			//lista = recibir_paquete(conexion);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			break;
		/*
		case FIN_F_TRUNCATE:
			recv_fin_f_truncate(conexion);
			log_info(logger, "FileSystem TRUNCO un archivo correctamente");

			break;

		case FIN_F_READ:
			recv_fin_f_read(conexion);
			log_info(logger, "FileSystem LEYO un archivo correctamente");

			break;

		case FIN_F_WRITE:
			recv_fin_f_write(conexion);
			log_info(logger, "FileSystem ESCRIBIO un archivo correctamente");

			break;
		*/
		case -1:
		log_info(logger, "Se desconectó FILESYSTEM");
					return;

				default:
					log_error(logger, "Respuesta de filesystem desconocida");
					continue;
		}
	}
}

void recibir_cpu_dispatch(int *conexion){

	int socket_dispatch = *conexion;
	t_contexto* contexto;
	t_pcb* pcb;

	while(true){
		switch(recibir_operacion(socket_dispatch)){

		case _DESALOJO: //Interrupcion fin de quantum o prioridades
			contexto = deserializar_contexto(socket_dispatch);
			log_info(logger, "Llegó contexto del  -> PID: < %d> por DESALOJO - Program_Counter: < %d >" , contexto->pid, contexto->program_counter);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;  //contexto_recibido, program counter actualizado
			exec_a_ready_PCB( pcb);
/*
			transicion_a_ready(pcb);
			sem_post(&pendientes_ready);
			sem_post(&cpu_libre);
			//sem_post(&pendientes_ready);
*/
			break;


		case _EXIT:
			contexto = deserializar_contexto(socket_dispatch);
			log_info(logger, "Llegó contexto del  -> PID: < %d> - Program_Counter: < %d >" , contexto->pid, contexto->program_counter);
			log_info(logger, "Valor final del registro AX < %d > ", obtenerValorRegistro("AX", contexto->registros) );
			log_info(logger, "Valor final del registro BX < %d > ", obtenerValorRegistro("BX", contexto->registros) );
			log_info(logger, "Valor final del registro CX < %d > ", obtenerValorRegistro("CX", contexto->registros) );
			log_info(logger, "Valor final del registro DX < %d > ", obtenerValorRegistro("DX", contexto->registros) );

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;
			pcb->motivo_fin = SUCCESS;
			transicion_exec_a_exit(pcb);
			break;


		case _SLEEP:
			contexto = deserializar_contexto(socket_dispatch);
			char* tiempo_de_bloqueo_string = recibir_valor(socket_dispatch);
			int tiempo_de_bloqueo = atoi(tiempo_de_bloqueo_string);
			free(tiempo_de_bloqueo_string);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;

			t_sleep_arg* args = malloc(sizeof(t_sleep_arg));
			args->pcb = pcb;
			args->tiempo = tiempo_de_bloqueo;

			pthread_t hilo_sleep;
			pthread_create(&hilo_sleep, NULL, (void *)atender_sleep, args);
			pthread_detach(hilo_sleep);
			break;

		case _WAIT:
			contexto = deserializar_contexto(socket_dispatch);
			char* recurso_wait = recibir_valor(socket_dispatch);
			log_info(logger, "Llegó contexto del  -> PID: %d - por WAIT del recurso: %s" , contexto->pid, recurso_wait);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;
			atender_wait(pcb, recurso_wait);
			free(recurso_wait);
			break;

		case _SIGNAL:
			contexto = deserializar_contexto(socket_dispatch);
			char* recurso_signal = recibir_valor(socket_dispatch);
			log_info(logger, "Llegó contexto del  -> PID: %d - por SIGNAL del recurso: %s" , contexto->pid, recurso_signal);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;
			atender_signal(pcb, recurso_signal);
			free(recurso_signal);
            break;

		case _PAGE_FAULT:
			contexto = deserializar_contexto(socket_dispatch);
			char* nro_pagina_string = recibir_valor(socket_dispatch);
			int nro_pagina = atoi(nro_pagina_string);
			free(nro_pagina_string);

			log_info(logger, "Llegó contexto del  -> PID: %d - por PAGE_FAULT de pág: %d" , contexto->pid, nro_pagina);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;

			t_fault_arg* fault_args = malloc(sizeof(t_fault_arg));
			fault_args->pcb = pcb;
			fault_args->nro_pagina=nro_pagina;

			pthread_t hilo_page_fault;
			pthread_create(&hilo_page_fault, NULL, (void *)atender_page_fault, fault_args);
			pthread_detach(hilo_page_fault);
            break;

		case _OPEN:
			contexto = deserializar_contexto(socket_dispatch);
			char* nombre_archivo_open = recibir_valor(socket_dispatch);
			char* tipo_lock_string = recibir_valor(socket_dispatch);
			code_lock tipo_lock = tipoLock(tipo_lock_string);
			free(tipo_lock_string);

			log_info(logger, "Llegó contexto del  -> PID: %d - F_OPEN del archivo: %s , modo: %s" , contexto->pid, nombre_archivo_open, lock_to_string(tipo_lock));

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;

			if(archivo_esta_abierto(nombre_archivo_open)){
				t_archivo* archivo = get_archivo_abierto(nombre_archivo_open);

				if(tipo_lock == WRITE || archivo->lock_activo == WRITE){
					log_info(logger, "Se BLOQUEA el proceso %d por F_OPEN: LOCK SOLICITADO: %s ; LOCK_ACTIVO: %s ; ARCHIVO: %s", pcb->contexto->pid, lock_to_string(tipo_lock), lock_to_string(archivo->lock_activo), archivo->nombre);
					cambiar_estado(pcb, BLOQUEADO);
					log_info(logger, "PID: %d - Bloqueado por: %s", pcb->contexto->pid, archivo->nombre);

					if(list_size(archivo->cola_block->cola) > 0 && tipo_lock == READ){
						t_peticion_archivo* ultima_peticion;
						pthread_mutex_lock(&(archivo->cola_block->mutex_cola));
						ultima_peticion = list_get(archivo->cola_block->cola, 0);
						pthread_mutex_unlock(&(archivo->cola_block->mutex_cola));
						if(ultima_peticion->tipo == READ){
							list_add(ultima_peticion->pcbs, pcb);
							log_info(logger, "PID: %d - Se encola lock READ", pcb->contexto->pid);
						}
						else{
							t_peticion_archivo* nueva_peticion = crear_peticion_archivo(pcb, tipo_lock);
							list_add_con_mutex(archivo->cola_block, nueva_peticion);
						}
					}
					else{
						t_peticion_archivo* nueva_peticion = crear_peticion_archivo(pcb, tipo_lock);
						list_add_con_mutex(archivo->cola_block, nueva_peticion);
					}
					list_add_con_mutex(cola_block, pcb);
					sem_post(&cpu_libre);
				}

				else if(tipo_lock == READ){
					log_info(logger, "Se AGREGA el proceso %d como “participante” del LOCK %s del ARCHIVO %s", pcb->contexto->pid, lock_to_string(archivo->lock_activo), archivo->nombre);
					t_archivo_proceso* archivo_proceso = crear_archivo_proceso(archivo);
					list_add(pcb->file_opens, archivo_proceso);
					list_add_con_mutex(cola_exec, pcb);
					enviarContexto(pcb->contexto,_CONTEXTO, socket_dispatch);
					//participantes_lock++;
					archivo->contador_de_aperturas++;
				}
			}

			else
				atender_f_open(pcb, nombre_archivo_open, tipo_lock);

			break;

		case _CLOSE:
			contexto = deserializar_contexto(socket_dispatch);
			char* nombre_archivo_close = recibir_valor(socket_dispatch);
			log_info(logger, "Llegó contexto del  -> PID: %d - F_CLOSE del archivo: %s" , contexto->pid, nombre_archivo_close);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;

			t_archivo* archivo = get_archivo_abierto(nombre_archivo_close);
			t_archivo_proceso* archivo_proceso = get_archivo_proceso(pcb->file_opens, nombre_archivo_close);
			list_remove_element(pcb->file_opens, archivo_proceso);
			destruir_archivo_proceso(archivo_proceso);
			archivo->contador_de_aperturas--;
			/*if(archivo->lock_activo == READ)
				participantes_lock--;*/

			if(archivo->contador_de_aperturas == 0){
				if(list_is_empty(archivo->cola_block->cola)){	//CHECK: mutex
					list_remove_element(tabla_archivos_abiertos, archivo);
					log_info(logger, "Se cierra el archivo %s", archivo->nombre);
					destruir_archivo(archivo);
				}
				else{
					t_pcb* pcb_lock;
					t_peticion_archivo* peticion = list_remove_con_mutex(archivo->cola_block);
					archivo->lock_activo = peticion->tipo;
					int cant_locks = list_size(peticion->pcbs);
					log_info(logger, "CANT LOCKS: %d", cant_locks);
					for(int i = 0; i < cant_locks; i++){
						pcb_lock = list_remove(peticion->pcbs, 0);
						t_archivo_proceso* nuevo_archivo_proceso = crear_archivo_proceso(archivo);
						list_add(pcb_lock->file_opens, nuevo_archivo_proceso);
						list_remove_pcb_con_mutex(cola_block, pcb_lock);
						list_add_con_mutex(cola_unlock, pcb_lock);
						sem_post(&pendientes_unlock);
						log_info(logger, "Se desbloquea el LOCK %s del PROCESO %d", lock_to_string(peticion->tipo), pcb_lock->contexto->pid);
						archivo->contador_de_aperturas++;
					}
					destruir_peticion_archivo(peticion);
				}
			}
			list_add_con_mutex(cola_exec, pcb);
			enviarContexto(pcb->contexto,_CONTEXTO, socket_dispatch);
			free(nombre_archivo_close);
            break;

		case _SEEK:
			contexto = deserializar_contexto(socket_dispatch);
			char* nombre_archivo_seek = recibir_valor(socket_dispatch);
			char* posicion_seek_string = recibir_valor(socket_dispatch);
			int posicion_seek = atoi(posicion_seek_string);

			log_info(logger, "Llegó contexto del PID: %d - F_SEEK del archivo: %s , POS: %d" , contexto->pid, nombre_archivo_seek ,posicion_seek);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;

			t_archivo_proceso* archivo_seek = get_archivo_proceso(pcb->file_opens, nombre_archivo_seek);
			// SUPONEMOS QUE EL ARCHIVO ESTA ABIERTO
			archivo_seek->puntero = posicion_seek;

			list_add_con_mutex(cola_exec, pcb);
			enviarContexto(pcb->contexto,_CONTEXTO, socket_dispatch);
			free(posicion_seek_string);
			free(nombre_archivo_seek);
            break;

		case _READ:
			contexto = deserializar_contexto(socket_dispatch);
			char* nombre_archivo_read = recibir_valor(socket_dispatch);
			char* direccion_fisica_read_string = recibir_valor(socket_dispatch);
			int direccion_fisica_read = atoi(direccion_fisica_read_string);
			log_info(logger, "Llegó contexto del  -> PID: %d - F_READ del archivo: %s ,DIRECCIÓN: %d" , contexto->pid, nombre_archivo_read ,direccion_fisica_read);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;

			cambiar_estado(pcb, BLOQUEADO);
			log_info(logger, "PID: %d - Bloqueado por: %s", pcb->contexto->pid, nombre_archivo_read);
			list_add_con_mutex(cola_block, pcb);
			sem_post(&cpu_libre);

			t_archivo_proceso* archivo_read = get_archivo_proceso(pcb->file_opens, nombre_archivo_read);

			int socket_read_fs = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
			send_f_read_fs(nombre_archivo_read, archivo_read->puntero, direccion_fisica_read, socket_read_fs, pcb->contexto->pid);

			t_peticion_fs_arg* read_args = malloc(sizeof(t_peticion_fs_arg));
			read_args->pcb = pcb;
			read_args->socket_peticion = socket_read_fs;

			pthread_t hilo_read_fs;
			pthread_create(&hilo_read_fs, NULL, (void *)atender_fin_peticion_fs, read_args);
			pthread_detach(hilo_read_fs);

			free(nombre_archivo_read);
			free(direccion_fisica_read_string);
			break;

		case _WRITE:	// TODO: EVITAR REPETIR LOGICA CON _READ
			contexto = deserializar_contexto(socket_dispatch);
			char* nombre_archivo_write = recibir_valor(socket_dispatch);
			char* direccion_fisica_write_string = recibir_valor(socket_dispatch);
			int direccion_fisica_write = atoi(direccion_fisica_write_string);
			log_info(logger, "Llegó contexto del  -> PID: %d - F_WRITE del archivo: %s ,DIRECCIÓN: %d" , contexto->pid, nombre_archivo_write, direccion_fisica_write);

			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;

			t_archivo* archivo_write = get_archivo_abierto(nombre_archivo_write);
			if(archivo_write->lock_activo == READ){
				pcb->motivo_fin = INVALID_WRITE;
				log_error(logger, "INVALID WRITE: PID: %d - ARCHIVO: %s", pcb->contexto->pid, nombre_archivo_write);
				transicion_exec_a_exit(pcb);
			}
			else if(archivo_write->lock_activo == WRITE){
				cambiar_estado(pcb, BLOQUEADO);
				log_info(logger, "PID: %d - Bloqueado por: %s", pcb->contexto->pid, nombre_archivo_write);
				list_add_con_mutex(cola_block, pcb);
				sem_post(&cpu_libre);

				t_archivo_proceso* archivo_write = get_archivo_proceso(pcb->file_opens, nombre_archivo_write);

				int socket_write_fs = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
				send_f_write_fs(nombre_archivo_write, archivo_write->puntero, direccion_fisica_write, socket_write_fs, pcb->contexto->pid);

				t_peticion_fs_arg* write_args = malloc(sizeof(t_peticion_fs_arg));
				write_args->pcb = pcb;
				write_args->socket_peticion = socket_write_fs;

				pthread_t hilo_write_fs;
				pthread_create(&hilo_write_fs, NULL, (void *)atender_fin_peticion_fs, write_args);
				pthread_detach(hilo_write_fs);
			}
			else
				log_error(logger, "ARCHIVO: %s no tienen ningun LOCK activo", nombre_archivo_write);


			free(nombre_archivo_write);
			free(direccion_fisica_write_string);
			break;

		case _TRUNCATE:
			contexto = deserializar_contexto(socket_dispatch);
			char* nombre_archivo_truncate = recibir_valor(socket_dispatch);
			char* nuevo_tamanio_string = recibir_valor(socket_dispatch);
			int nuevo_tamanio = atoi(nuevo_tamanio_string);
			log_info(logger, "Llegó contexto del  -> PID: %d - F_TRUNCATE del archivo: %s ,TAMANIO: %d" , contexto->pid, nombre_archivo_truncate ,nuevo_tamanio);


			pcb = list_remove_con_mutex(cola_exec);
			liberarContexto(pcb->contexto);
			pcb->contexto = contexto;

			cambiar_estado(pcb, BLOQUEADO);
			log_info(logger, "PID: %d - Bloqueado por: %s", pcb->contexto->pid, nombre_archivo_truncate);
			list_add_con_mutex(cola_block, pcb);
			sem_post(&cpu_libre);

			int socket_truncate_fs = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
			send_f_truncate_fs(nombre_archivo_truncate, nuevo_tamanio, socket_truncate_fs);

			t_peticion_fs_arg* truncate_args = malloc(sizeof(t_peticion_fs_arg));
			truncate_args->pcb = pcb;
			truncate_args->socket_peticion = socket_truncate_fs;

			pthread_t hilo_truncate_fs;
			pthread_create(&hilo_truncate_fs, NULL, (void *)atender_fin_peticion_fs, truncate_args);
			pthread_detach(hilo_truncate_fs);

			free(nombre_archivo_truncate);
			free(nuevo_tamanio_string);
			break;

		case -1:
			log_info(logger, "Se desconectó CPU Dispatch");
			return;

		default:
			log_error(logger, "Respuesta de cpu dispatch desconocida");
			continue;
		}
	}
}


void enviar_cpu_interrupt(int socket_interrupt){
//Usar para enviar interrupción, pero hay que mantener los hilos conectados

}


void atender_sleep(t_sleep_arg* args){
	cambiar_estado(args->pcb, BLOQUEADO);
	log_info(logger, "PID: %d - Bloqueado por: SLEEP , tiempo : %d", args->pcb->contexto->pid, args->tiempo);
	list_add_con_mutex(cola_block, args->pcb);
	sem_post(&cpu_libre);

	sleep(args->tiempo);

	list_remove_pcb_con_mutex(cola_block, args->pcb);
	list_add_con_mutex(cola_unlock, args->pcb);
	sem_post(&pendientes_unlock);
	free(args);
}


void atender_page_fault(t_fault_arg* arg){

   //Pasar a bloqueado
	cambiar_estado(arg->pcb, BLOQUEADO);
	log_info(logger, "PID: %d - Bloqueado por: PAGE_FAULT , pagina: %d", arg->pcb->contexto->pid, arg->nro_pagina);
	list_add_con_mutex(cola_block, arg->pcb);
	sem_post(&cpu_libre);


    // SOLICITUD A MEMORIA PARA CARGAR LA PAGINA DEL PROCESO
	pthread_mutex_lock(&mutex_page_fault);
	t_paquete* paquete= crear_paquete(_PAGE_FAULT);
	agregar_a_paquete(paquete, &arg->pcb->contexto->pid, sizeof(int) );
	agregar_a_paquete(paquete, &arg->nro_pagina, sizeof(int) );
	enviar_paquete(paquete, socket_memoria);

	char* pid_respuesta = recibir_valor(socket_memoria);
	pthread_mutex_unlock(&mutex_page_fault);

	if(arg->pcb->contexto->pid != atoi(pid_respuesta))
		log_error(logger, "ERROR EN COMUNICACION KERNEL-MEMORIA: PID SOLICITANTE: %d != DE PID RECIBIDO: %s", arg->pcb->contexto->pid, pid_respuesta);

   	log_info(logger, "PID: %d - Se cargó la PAGINA: %d", arg->pcb->contexto->pid, arg->nro_pagina);

   	list_remove_pcb_con_mutex(cola_block, arg->pcb);
   	list_add_con_mutex(cola_unlock, arg->pcb);
   	sem_post(&pendientes_unlock);
   	free(pid_respuesta);
   	free(arg);
}


void atender_fin_peticion_fs(t_peticion_fs_arg* args){
	t_pcb* pcb = args->pcb;
	int socket_peticion = args->socket_peticion;

	char* mensaje = recibir_valor(socket_peticion); // SE BLOQUEA EL HILO ESPERANDO EL OK DE FS
	free(mensaje);

	list_remove_pcb_con_mutex(cola_block, pcb);
	list_add_con_mutex(cola_unlock, args->pcb);
	sem_post(&pendientes_unlock);
	liberar_conexion(socket_peticion);
	free(args);
}


void send_f_read_fs(char* nombre_archivo, int puntero_seek, int direccion_fisica, int socket_peticion, int pid){
	int tamanio_nombre = strlen(nombre_archivo) + 1;

	t_paquete* paquete = crear_paquete(_READ);
	agregar_a_paquete(paquete, nombre_archivo, tamanio_nombre);
	agregar_a_paquete(paquete, &puntero_seek, sizeof(int));
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
	agregar_a_paquete(paquete, &pid, sizeof(int));
	enviar_paquete(paquete, socket_peticion);
}


void send_f_write_fs(char* nombre_archivo, int puntero_seek, int direccion_fisica, int socket_peticion, int pid){
	int tamanio_nombre = strlen(nombre_archivo) + 1;

	t_paquete* paquete = crear_paquete(_WRITE);
	agregar_a_paquete(paquete, nombre_archivo, tamanio_nombre);
	agregar_a_paquete(paquete, &puntero_seek, sizeof(int));
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
	agregar_a_paquete(paquete, &pid, sizeof(int));
	enviar_paquete(paquete, socket_peticion);
}


void send_f_truncate_fs(char* nombre_archivo, int nuevo_tamanio, int socket_peticion){
	int tamanio_nombre = strlen(nombre_archivo) + 1;

	t_paquete* paquete = crear_paquete(_TRUNCATE);
	/*agregar_a_paquete(paquete, nombre_archivo, tamanio_nombre);
	agregar_a_paquete(paquete, &nuevo_tamanio, sizeof(int));
		*/

	//t_buffer* buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = tamanio_nombre + 2 * sizeof(int)  ; //char + int tamanio + int del largo
	paquete->buffer->stream = malloc(paquete->buffer->size);

   int desplazamiento= 0;
   memcpy(paquete->buffer->stream + desplazamiento, &nuevo_tamanio, sizeof(int));
   desplazamiento+= sizeof(int);
   memcpy(paquete->buffer->stream + desplazamiento, &tamanio_nombre, sizeof(int));  //tamaño del char
   desplazamiento+= sizeof(int);
   memcpy(paquete->buffer->stream + desplazamiento, nombre_archivo, tamanio_nombre);  //char
	//paquete->buffer = buffer;


	enviar_paquete(paquete, socket_peticion);

}


void enviar_peticion_open(char* nombre_archivo){

	int socket_open_f = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
	t_paquete* paquete = crear_paquete(_OPEN);
	int tamanio_nombre = strlen(nombre_archivo) + 1;

	/*t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = tamanio_nombre + 1 * sizeof(int)  ; //char + int tamanio
	buffer->stream = malloc(buffer->size);

	int desplazamiento= 0;
	memcpy(buffer->stream + desplazamiento, &tamanio_nombre, sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(buffer->stream + desplazamiento, nombre_archivo, tamanio_nombre);  //char
	paquete->buffer = buffer;*/


	agregar_a_paquete(paquete, nombre_archivo, tamanio_nombre);
	enviar_paquete(paquete, socket_open_f);
	// RECIBIR OK?
	liberar_conexion(socket_open_f);
}
