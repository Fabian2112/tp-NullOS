/*
 * recursos.c
 *
 *  Created on: Oct 20, 2023
 *      Author: utnso
 */

#include "../include/recursos.h"

/* 	===============================================================================================================
					RECURSOS
	===============================================================================================================*/

void inicializar_recursos(){
	char** nombres_recursos = recursos();
	int cant_recursos = string_array_size(nombres_recursos);

	lista_recursos = list_create();

	char** instancias = instancias_recursos();

	for(int i = 0; i < cant_recursos; i++){
		t_recurso* recurso = malloc(sizeof(t_recurso));

		recurso->id = i;

		char* nombre = nombres_recursos[i];
		recurso->nombre = malloc(sizeof(char) * strlen(nombre) + 1);
		strcpy(recurso->nombre, nombre);

		recurso->instancias = atoi(instancias[i]);

		recurso->cola_block = monitor_create();

		recurso->cola_asignados = monitor_create();

		list_add(lista_recursos, recurso);
	}
	string_array_destroy(nombres_recursos);
	string_array_destroy(instancias);
}


bool recurso_existe(char* nombre){
	return get_recurso(nombre) != NULL;
}


t_recurso* get_recurso(char* nombre){
	int cant_recursos = list_size(lista_recursos);
	t_recurso* recurso;

	for(int i = 0; i < cant_recursos; i++){
		recurso = list_get(lista_recursos, i);
		if(string_equals_ignore_case(recurso->nombre, nombre)){
			return recurso;
		}
	}
	return NULL;
}


void liberar_recursos(){
	list_destroy_and_destroy_elements(lista_recursos, (void*)liberar_recurso);
}


void liberar_recurso(void* recurso){
	free(((t_recurso*)recurso)->nombre);
	monitor_destroy(((t_recurso*)recurso)->cola_block);
	monitor_destroy(((t_recurso*)recurso)->cola_asignados);
	free(((t_recurso*)recurso));
}


void atender_wait(t_pcb* pcb, char* recurso){
	//t_recurso* recurso_solicitado = buscar_recurso_por_nombre(recurso);
	//if(recurso_solicitado->id == -1)
	if(!recurso_existe(recurso)){
		pcb->motivo_fin = INVALID_RESOURCE;
		log_error(logger, "PID: %d solicito el recurso inexistente: %s", pcb->contexto->pid, recurso);
		transicion_exec_a_exit(pcb);
	}
	else{
		t_recurso* recurso_solicitado = get_recurso(recurso);
		recurso_solicitado->instancias--;
		log_info(logger, "PID: %d - Wait: %s - Instancias: %d", pcb->contexto->pid, recurso, recurso_solicitado->instancias);
		if(recurso_solicitado->instancias < 0){
			cambiar_estado(pcb, BLOQUEADO);
			log_info(logger, "PID: %d - Bloqueado por: %s", pcb->contexto->pid, recurso);
			list_add_con_mutex(recurso_solicitado->cola_block, pcb);
			list_add_con_mutex(cola_block, pcb);
			detectar_deadlock();
			sem_post(&cpu_libre);
		}
		else{
			list_add_con_mutex(recurso_solicitado->cola_asignados, pcb);
			list_add_con_mutex(cola_exec, pcb);
			enviarContexto(pcb->contexto,_CONTEXTO, socket_dispatch);
		}
	}
}


void atender_signal(t_pcb* pcb, char* recurso){
	//t_recurso* recurso_a_liberar = buscar_recurso_por_nombre(recurso);
	//if(recurso_a_liberar->id == -1){
	if(!recurso_existe(recurso)){
		pcb->motivo_fin = INVALID_RESOURCE;
		log_error(logger, "PID: %d trato de liberar el recurso inexistente: %s", pcb->contexto->pid, recurso);
		transicion_exec_a_exit(pcb);
	}
	else{
		t_recurso* recurso_a_liberar = get_recurso(recurso);
		if(!proceso_tiene_asignado(pcb, recurso_a_liberar)){
			pcb->motivo_fin = INVALID_SIGNAL;
			log_error(logger, "PID: %d trato de liberar un recurso que no tiene asignado: %s", pcb->contexto->pid, recurso);
			transicion_exec_a_exit(pcb);
		}
		else{
			list_remove_pcb_con_mutex(recurso_a_liberar->cola_asignados, pcb);
			recurso_a_liberar->instancias++;
			log_info(logger, "PID: %d - Signal: %s - Instancias: %d", pcb->contexto->pid, recurso, recurso_a_liberar->instancias);
			if(recurso_a_liberar->instancias <= 0){
				t_pcb* pcb_block = list_remove_con_mutex(recurso_a_liberar->cola_block);
				list_remove_pcb_con_mutex(cola_block, pcb_block);
				list_add_con_mutex(recurso_a_liberar->cola_asignados, pcb_block);
				list_add_con_mutex(cola_unlock, pcb_block);
				sem_post(&pendientes_unlock);
			}
			list_add_con_mutex(cola_exec, pcb);
			enviarContexto(pcb->contexto,_CONTEXTO, socket_dispatch);
		}
	}
}


bool proceso_tiene_asignado(t_pcb* pcb, t_recurso* recurso){
	int cant_asignados = list_size(recurso->cola_asignados->cola);
	for(int i = 0; i < cant_asignados; i++){
		t_pcb* pcb_asignado = list_get(recurso->cola_asignados->cola, i);
		if(pcb->contexto->pid == pcb_asignado->contexto->pid){
			return true;
		}
	}
	return false;
}


bool proceso_esta_pidiendo(t_pcb* pcb, t_recurso* recurso){
	int cant_bloqueados = list_size(recurso->cola_block->cola);

	for(int i = 0; i < cant_bloqueados; i++){
		t_pcb* pcb_bloqueado = list_get(recurso->cola_block->cola, i);
		if(pcb->contexto->pid == pcb_bloqueado->contexto->pid){
			return true;
		}
	}
	return false;
}


/* 	===============================================================================================================
					ARCHIVOS
	===============================================================================================================*/

void inicializar_tabla_archivos(){
	tabla_archivos_abiertos = list_create();
	//participantes_lock = 0;
}

void liberar_tabla_archivos(){
	list_destroy_and_destroy_elements(tabla_archivos_abiertos, (void*)destruir_archivo);
}

void atender_f_open(t_pcb* pcb, char* nombre_archivo, code_lock tipo_lock){

	t_archivo* archivo = crear_archivo(nombre_archivo);

	//send_abrir_archivo(nombre_archivo, socket_filesystem);
	// recv_abrir_archivo(socket_filesystem);	// DEVUELVE siempre OK (tamaño del archivo)
	// No me interesa la respuesta asi que el recv capaz puede estar en otro hilo.

	enviar_peticion_open(nombre_archivo);

	archivo->lock_activo = tipo_lock;
	archivo->contador_de_aperturas++;

	list_add(tabla_archivos_abiertos, archivo);	// CHECK: creo que no, pero capaz deberia sincronizarlo (mutex_archivos_abiertos)

	t_archivo_proceso* archivo_proceso = crear_archivo_proceso(archivo);
	list_add(pcb->file_opens, archivo_proceso); // CHECK: idem

	log_info(logger, "El proceso %d CREA un LOCK %s en el ARCHIVO %s", pcb->contexto->pid, lock_to_string(tipo_lock), archivo->nombre);

	list_add_con_mutex(cola_exec, pcb);
	enviarContexto(pcb->contexto,_CONTEXTO, socket_dispatch);
}


t_archivo* get_archivo_abierto(char* nombre){
	int cant_archivos = list_size(tabla_archivos_abiertos);
	t_archivo* archivo;

	for(int i = 0; i < cant_archivos; i++){
		archivo = list_get(tabla_archivos_abiertos, i);
		if(string_equals_ignore_case(archivo->nombre, nombre)){
			return archivo;
		}
	}
	return NULL;
}


bool archivo_esta_abierto(char* nombre){
	return get_archivo_abierto(nombre) != NULL;
}


code_lock tipoLock(char* tipo){
	if(string_equals_ignore_case(tipo,"R"))
		return READ;
	else if(string_equals_ignore_case(tipo,"W"))
		return WRITE;
	else{
		log_info(logger, "Tipo de lock desconocido");
		return -1;
	}
}


const char* lock_to_string(code_lock tipo){
	switch(tipo){
	case READ:
		return "READ"; break;
	case WRITE:
		return "WRITE"; break;
	default:
		return "ERROR"; break;
	}
}


t_archivo* crear_archivo(char* nombre_archivo){
	t_archivo* archivo = (t_archivo*)malloc(sizeof(t_archivo));
	archivo->nombre = nombre_archivo;
	archivo->cola_block = monitor_create();
	archivo->contador_de_aperturas = 0;

	return archivo;
}


void destruir_archivo(t_archivo* archivo){
	monitor_destroy(archivo->cola_block);
	free(archivo);
}


t_archivo_proceso* crear_archivo_proceso(t_archivo* archivo){
	t_archivo_proceso* archivo_proceso = (t_archivo_proceso*)malloc(sizeof(t_archivo_proceso));
	archivo_proceso->nombre = archivo->nombre;
	archivo_proceso->puntero = 0;

	return archivo_proceso;
}


t_archivo_proceso* get_archivo_proceso(t_list* file_opens, char* nombre){
	int cant_archivos = list_size(file_opens);
	t_archivo_proceso* archivo;

	for(int i = 0; i < cant_archivos; i++){
		archivo = list_get(file_opens, i);
		if(string_equals_ignore_case(archivo->nombre, nombre)){
			return archivo;
		}
	}
	return NULL;
}


t_peticion_archivo* crear_peticion_archivo(t_pcb* pcb, code_lock tipo_lock){
	t_peticion_archivo* peticion = (t_peticion_archivo*)malloc(sizeof(t_peticion_archivo));
	peticion->pcbs = list_create();
	list_add(peticion->pcbs, pcb);
	peticion->tipo = tipo_lock;

	return peticion;
}


void destruir_archivo_proceso(t_archivo_proceso* archivo_proceso){
	free(archivo_proceso);
}


void destruir_peticion_archivo(t_peticion_archivo* peticion){
	list_destroy(peticion->pcbs);
	free(peticion);
}


t_peticion_archivo* obtener_peticion_archivo(t_pcb* pcb, t_archivo* archivo){	// SOLO PARA UNA PETICION
	int cant_peticiones = list_size(archivo->cola_block->cola);
	t_peticion_archivo* peticion;

	for(int i = 0; i < cant_peticiones; i++){
		peticion = list_get(archivo->cola_block->cola, i);
		cant_peticiones = list_size(peticion->pcbs);
		t_pcb* pcb_lock;
		for(int j = 0; j < cant_peticiones; j++){
			pcb_lock = list_get(peticion->pcbs, j);
			if(pcb_lock->contexto->pid == pcb->contexto->pid)
				return peticion;
		}
	}
	return NULL;
}



/* 	===============================================================================================================
					LIBERAR RECURSOS AL FINALIZAR PROCESO
	===============================================================================================================*/


void liberar_recursos_pcb(t_pcb* pcb){
	// ARCHIVOS:
	if(!list_is_empty(pcb->file_opens)){
		int cant_archivos_abiertos = list_size(pcb->file_opens);

		for(int i = 0; i < cant_archivos_abiertos; i++){
			t_archivo_proceso* archivo_proceso = list_get(pcb->file_opens, i);
			simular_f_close(pcb, archivo_proceso);
		}
	}

	// REVIERTO PETICIONES DE ARCHIVOS:
	t_archivo* archivo;
	int cant_archivos = list_size(tabla_archivos_abiertos);
	for(int i = 0; i < cant_archivos; i++){
		archivo = list_get(tabla_archivos_abiertos, i);
		t_peticion_archivo* peticion = obtener_peticion_archivo(pcb, archivo);
		if(peticion != NULL){
			if(peticion->tipo == READ){
				list_remove_element(peticion->pcbs, pcb);
				if(list_is_empty(peticion->pcbs))
					destruir_peticion_archivo(peticion);
			}
			else if (peticion->tipo == WRITE){
				pthread_mutex_lock(&(archivo->cola_block->mutex_cola));
				list_remove_element(archivo->cola_block->cola, peticion);
				pthread_mutex_unlock(&(archivo->cola_block->mutex_cola));
				destruir_peticion_archivo(peticion);
			}
		}
	}

	// RECURSOS:
	t_recurso* recurso;
	int cant_recursos = list_size(lista_recursos);
	for(int i = 0; i < cant_recursos; i++){
		recurso = list_get(lista_recursos, i);
		if(proceso_tiene_asignado(pcb, recurso)){
			list_remove_pcb_con_mutex(recurso->cola_asignados, pcb); // REMOVER PCB DE COLA_ASIGNADOS
			simular_signal(recurso);
			log_info(logger, "Libero Recurso: RECURSO: %s - Instancias: %d - PID: %d", recurso->nombre, recurso->instancias, pcb->contexto->pid);
		}
		if(proceso_esta_pidiendo(pcb, recurso)){
			recurso->instancias++;
			list_remove_pcb_con_mutex(recurso->cola_block, pcb); // REMOVER PCB DE COLA_BLOCK
		}
	}
}


void simular_f_close(t_pcb* pcb, t_archivo_proceso* archivo_proceso){
	t_archivo* archivo = get_archivo_abierto(archivo_proceso->nombre);
	list_remove_element(pcb->file_opens, archivo_proceso);
	destruir_archivo_proceso(archivo_proceso);
	archivo->contador_de_aperturas--;

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
			for(int i = 0; i < cant_locks; i++){
				pcb_lock = list_remove(peticion->pcbs, i);
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
}


void simular_signal(t_recurso* recurso){
	recurso->instancias++;

	if(recurso->instancias <= 0){
		t_pcb* pcb_block = list_remove_con_mutex(recurso->cola_block);
		list_remove_pcb_con_mutex(cola_block, pcb_block);
		list_add_con_mutex(recurso->cola_asignados, pcb_block);
		list_add_con_mutex(cola_unlock, pcb_block);
		sem_post(&pendientes_unlock);
	}
}


/* 	===============================================================================================================
					DEADLOCK
	===============================================================================================================*/

void detectar_deadlock(){
	log_info(logger, "ANALISIS DE DETECCION DE DEADLOCK");

	int cant_recursos = list_size(lista_recursos);
	int cant_procesos = list_size(cola_general->cola);
	log_info(logger, "Cantidad de Recursos: %d - Cantidad de procesos: %d", cant_recursos, cant_procesos);

	int peticiones[cant_procesos][cant_recursos];
	inicializar_peticiones(cant_procesos, cant_recursos, peticiones);
	log_info(logger, "INICIALIZO PETICIONES");
	logear_matriz(cant_procesos, cant_recursos, peticiones);

	int asignaciones[cant_recursos][cant_procesos];
	inicializar_asignaciones(cant_recursos, cant_procesos, asignaciones);
	log_info(logger, "INICIALIZO ASIGNACIONES");
	logear_matriz(cant_recursos, cant_procesos, asignaciones);

	int transiciones[cant_procesos][cant_procesos];
	multiplicar_matrices(cant_recursos, cant_procesos, transiciones, asignaciones, peticiones);


	floydwarshall(cant_procesos, transiciones);

	log_info(logger, "DETECTANDO ESPERA CIRCULAR");
	detectar_espera_circular(cant_procesos, transiciones);
}


void logear_matriz(int filas, int columnas, int matriz[filas][columnas]){
	log_info(logger, "PRINTEANDO MATRIZ");
	for(int i = 0; i < filas; i++){
	    for(int j = 0; j < columnas; j++){
	         printf("%d     ", matriz[i][j]);
	    }
	    printf("\n");
	}
}


void detectar_espera_circular(int procesos, int transiciones[procesos][procesos]){
	for (int i = 0; i < procesos; i++){
		for (int j = 0; j < procesos; j++){
			if(i == j && transiciones[i][j] == 1){
				t_pcb* pcb = list_get(cola_general->cola, i);




				log_info(logger, "Deadlock detectado: PID: %d", pcb->contexto->pid);
			}
		}
	}
}


void inicializar_peticiones(int procesos, int recursos, int peticiones[procesos][recursos]){
	for(int i = 0; i < procesos; i++){
		t_pcb* pcb = list_get(cola_general->cola, i);

		for(int j = 0; j < recursos; j++){
			t_recurso* recurso = list_get(lista_recursos, j);

			//if(j == 0)
				//peticiones[i][j] = pcb->contexto->pid;
			//else{
				// SI EL PCB ESTA BLOQUEADO EN EL RECURSO J
				if(proceso_esta_pidiendo(pcb, recurso))
					peticiones[i][j] = 1;
				else
					peticiones[i][j] = 0;
			//}
		}
	}
}



void inicializar_asignaciones(int recursos, int procesos, int asignaciones[recursos][procesos]){
	for(int i = 0; i < recursos; i++){
		t_recurso* recurso = list_get(lista_recursos, i);

		for(int j = 0; j < procesos; j++){
			t_pcb* pcb = list_get(cola_general->cola, j);

			//if(i == 0)
			//	asignaciones[i][j] = pcb->contexto->pid;
			//else{
				// SI EL PCB ESTA BLOQUEADO EN EL RECURSO J
				if(proceso_tiene_asignado(pcb, recurso))
					asignaciones[i][j] = 1;
				else
					asignaciones[i][j] = 0;
			//}
		}
	}
}


void multiplicar_matrices
(int recursos,
		int procesos,
		int transiciones[procesos][procesos],
		int asignaciones[recursos][procesos],
		int peticiones[procesos][recursos]){

	for (int i = 0; i < procesos; i++){
		for (int j = 0; j < procesos; j++){
			//if(i == 0)
			//	transiciones[i][j] = asignaciones[i][j];

			int multiplicacion = 0;
			for (int k = 0; k < recursos; k++){
				multiplicacion = (peticiones[i][k] && asignaciones[k][j]) || multiplicacion;
			}
			transiciones[i][j] =  multiplicacion;
		}
	}
}


void floydwarshall(int procesos, int transiciones[procesos][procesos]){
	for (int k = 0; k < procesos; k++){
	    for (int i = 0; i < procesos; i++){
	        for (int j = 0; j < procesos; j++){
	        	transiciones[i][j] = transiciones[i][j] || (transiciones[i][k] && transiciones[k][j]);
	        }
	    }
	}
}


/* 	===============================================================================================================
					UTILES
	===============================================================================================================*/


int* string_to_int_array(char** array_de_strings){
	int count = string_array_size(array_de_strings);
	int *numbers = malloc(sizeof(int) * count);
	for(int i = 0; i < count; i++){
		int num = atoi(array_de_strings[i]);
		numbers[i] = num;
	}
	return numbers;
}



