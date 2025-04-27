/*
 * consola.c
 *
 *  Created on: Oct 14, 2023
 *      Author: utnso
 */
#include "../include/consola.h"

void iniciar_consola(){

	 char *leido;
	 bool finalizar = false;

		    while (!finalizar) {
		    	leido = readline("Ingrese mensaje \n ");
		        if (!leido) {
		            break;
		        }
		        if (!strcmp(leido,"FIN")) {  //cuando se ingresa la palabra FIN, se corta la ejecución de mensaje_consola
		        	finalizar = true;
		            free(leido);
		            break;
		        }
		        log_info(logger, "Se ingresó siguiente mensaje: %s \n", leido);
		        mensaje(leido);
		        free(leido);
	    }

}

void mensaje(char* mensaje){
	//obtener cód de mensaje
	char* cod_mensaje = obtenerCodigo(mensaje);
	//log_info(logger, "Código del mensaje: %s \n", cod_mensaje);

	if(string_equals_ignore_case(cod_mensaje,"INICIAR_PROCESO") )
		iniciar_proceso(mensaje);
	else if(string_equals_ignore_case(cod_mensaje,"FINALIZAR_PROCESO") )
		finalizar_proceso(mensaje);
	else if(string_equals_ignore_case(cod_mensaje,"DETENER_PLANIFICACION") )
		detener_planificacion();
	else if(string_equals_ignore_case(cod_mensaje,"INICIAR_PLANIFICACION") )
		iniciar_planificacion();
	else if(string_equals_ignore_case(cod_mensaje,"MULTIPROGRAMACION") )
		multiprogramacion(mensaje);
	else if(string_equals_ignore_case(cod_mensaje,"PROCESO_ESTADO") )
		proceso_estado();
	else {
		log_info(logger, "Mensaje Inválido \n");
		}

	free(cod_mensaje);

}

void iniciar_proceso(char* mensaje){
	char** parametros;
	//parámetros del mensaje
	char* path;
	uint32_t size = 0;
	uint32_t prioridad = 0;

	//Verifico cant. parámetros
    parametros = obtenerParametros(mensaje, 3); //path, size, prioridad

    if (parametros == NULL)
    	log_info(logger, "Error cantidad de parámetros esperados \n");
    else{
    	path= parametros[1]; //log_info(logger, "Path: %s \n", path);
    	size= atoi(parametros[2]); //log_info(logger, "Size: %d \n", size);
    	prioridad= atoi(parametros[3]); //log_info(logger, "Prioridad: %d \n", prioridad);

    	//Obtener el nombre del archivo
    	char* nombre_archivo = obtenerNombreArchivo(path);
    	log_info(logger, "Nombre del archivo: %s \n", nombre_archivo);

    	t_pcb* pcb = crearPCB(size, prioridad, nombre_archivo);

    	agregarNEW(pcb);

    	liberarParametros(parametros);
    }
}

void finalizar_proceso(char* mensaje){
	char** parametros;
	//parámetros del mensaje
	uint32_t pid = 0;

	//Verifico cant. parámetros
    parametros = obtenerParametros(mensaje, 1);

    if (parametros == NULL)
    	log_info(logger, "Error cantidad de parámetros esperados \n");
    else{
    	pid= atoi(parametros[1]); log_info(logger, "PID: %d \n", pid);

    	finalizar_id(pid);

    	liberarParametros(parametros);
    }
}

void detener_planificacion(){
	sem_wait(&detenerPlanificacion);
	log_info(logger,"PAUSA DE PLANIFICACIÓN");
}

void iniciar_planificacion(){
	sem_post(&detenerPlanificacion);
	log_info(logger,"INICIAR PLANIFICACIÓN");
}
void multiprogramacion(char* mensaje){
	char** parametros;
	//parámetros del mensaje
	uint32_t valor = 0;

	//Verifico cant. parámetros
    parametros = obtenerParametros(mensaje, 1);

    if (parametros == NULL)
    	log_info(logger, "Error cantidad de parámetros esperados \n");
    else{
    	valor = atoi(parametros[1]); log_info(logger, "Valor: %d \n", valor);


    	pthread_mutex_lock(&mutexGradoMultiprogramacion);
    	 int nuevo_grado = gradoMultiprogramacion + valor;
    	 log_info(logger, "Grado Anterior: %d - Grado Actual: %d ", gradoMultiprogramacion, nuevo_grado );
    	 gradoMultiprogramacion = nuevo_grado;

    	 sem_multiprogramacion.__align = nuevo_grado;

    	 pthread_mutex_unlock(&mutexGradoMultiprogramacion);

     	liberarParametros(parametros);
    }
}

void proceso_estado(){
	imprimir_colas(colas_new, NUEVO);
	imprimir_colas(cola_ready, LISTO);
	imprimir_colas(cola_exec, EJECUTANDO);
	imprimir_colas(cola_block, BLOQUEADO);
	imprimir_colas(cola_unlock, BLOQUEADO); // ESTADO INTERMEDIO ENTRE BLOCK Y READY ==> SUPONEMOS QUE SIGUE EN BLOCK
	//imprimir_colas(cola_exit,FINALIZADO);

}

void imprimir_colas(t_monitor* monitor, code_estado estado){

	pthread_mutex_lock(&(monitor->mutex_cola));

	const char* estado_actual =   estadoToString(estado);
	int cantidad = list_size(monitor->cola); log_info(logger,"Cant. de procesos: %d", cantidad);
	char *procesos = string_new();

		if(cantidad  == 0 ){
			log_info(logger, "Estado: %s - No hay procesos", estado_actual);
		}
		else{
			for (int i = 0; i < cantidad; i++) {
				t_pcb* pcb = list_get(monitor->cola , i);
				char* pid = string_itoa(pcb->contexto->pid);
			    if(i == cantidad -1)
			    	string_append(&procesos , pid  );
			    else{
			    	string_append(&procesos , pid  );
			    	string_append(&procesos , " , " );
			    }
			    free(pid);
			}

		log_info(logger, "Estado: %s - Procesos :< %s >" , estado_actual, procesos);

		}
		free(procesos);
		pthread_mutex_unlock(&(monitor->mutex_cola));
}


void finalizar_id(int pid){
	t_pcb* pcb_buscado = NULL;

	// BUSCAR EN NEW
	pcb_buscado =esta_pcb_estado(colas_new, pid);
	if(pcb_buscado != NULL){
		log_info(logger,"Proceso a finalizar se encuentra en NEW");
		finalizar_new_exit(pcb_buscado);
	}
	// BUSCAR EN READY
	if(pcb_buscado == NULL){
		pcb_buscado = esta_pcb_estado(cola_ready, pid);
		if(pcb_buscado != NULL) {
			//pthread_mutex_lock(&(cola_ready->mutex_cola));
			log_info(logger,"Proceso a finalizar se encuentra en READY");
			finalizar_ready_exit(pcb_buscado);
		}
	}

	// BUSCAR EN BLOCK
	if(pcb_buscado == NULL){
		pcb_buscado = esta_pcb_estado(cola_block, pid);
		if(pcb_buscado != NULL) {
			log_info(logger,"Proceso a finalizar se encuentra en BLOQUEADO");
	  		finalizar_block_exit(pcb_buscado);
		}
	}

	// BUSCAR EN UNLOCK
	if(pcb_buscado == NULL){
		pcb_buscado = esta_pcb_estado(cola_unlock, pid);
		if(pcb_buscado != NULL) {
			log_info(logger,"Proceso a finalizar se encuentra en BLOQUEADO (pre-unlock)");
		  	finalizar_unlock_exit(pcb_buscado);
		}
	}

	// BUSCAR EN EXEC
	if(pcb_buscado == NULL){
		pcb_buscado = esta_pcb_estado(cola_exec, pid);
		if(pcb_buscado != NULL) {
			log_info(logger,"Proceso a finalizar se encuentra en EXEC");
			finalizar_exec_exit(pcb_buscado);
		}
		else
			log_info(logger, "No se encuentra el proceso: %d en ningún estado activo", pid);
	}
}


t_pcb* esta_pcb_estado(t_monitor* monitor, int pid) {
		t_list * lista = monitor->cola;

		for (int i = 0; i < list_size(lista); i++) {
	        t_pcb* pcb_buscado = list_get(lista, i);

	        if (pcb_buscado->contexto->pid == pid) {
	            return pcb_buscado;   //indice
	        }
	    }
	    //NO ESTÁ
	    return NULL;
	}





char* obtenerNombreArchivo(char* linea){
	char** parametros = string_split(linea, "/");
	int posicion= 0;
	while(parametros[posicion] != NULL){
		posicion = posicion + 1;
	}

	int ultimo = string_array_size(parametros) -1;

	char* nombre_archivo = malloc(sizeof(char) * strlen(parametros[ultimo]) + 1);
	strcpy(nombre_archivo, parametros[ultimo]);

	liberarParametros(parametros);
	return nombre_archivo;
}

char*  obtenerCodigo(char* linea_mensaje){
	char* codigo;
		char** parseo = string_split(linea_mensaje, " ");

		if(parseo[0] == NULL)
		{
			codigo =  NULL;
		}
		else
		{
			codigo = strdup(parseo[0]);
		}

	     // Liberar memoria del Split , definición en shared
		liberarParametros(parseo);

		return codigo;
}


