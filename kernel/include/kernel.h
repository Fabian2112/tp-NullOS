/*
 * kernel.h
 *
 *  Created on: Sep 22, 2023
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "conexiones_kernel.h"
#include "planificacion.h"
#include "consola.h"
#include "recursos.h"


t_log* logger;
t_config* config;


// VARIABLES DE CONEXION:

int socket_memoria;
int socket_filesystem;
int socket_dispatch;
int socket_interrupt;

// HILOS DE CONEXION

pthread_t conexion_memoria;
pthread_t conexion_filesystem;
pthread_t conexion_cpu_dispatch;
pthread_t conexion_cpu_interrupt;

// RECURSOS

t_list* lista_recursos;

// ARCHIVOS:

t_list* tabla_archivos_abiertos;


// COLAS DE PLANIFICACION (con monitores)
t_monitor* cola_general;
t_monitor* colas_new;
t_monitor* cola_ready;
t_monitor* cola_exec;
t_monitor* cola_unlock; // CAMBIAR sem pendientes_block_return
t_monitor* cola_block;
t_monitor* cola_exit;


// VARIABLES

int id_proceso; // CONTADOR DE PIDS
int gradoMultiprogramacion;
bool desalojo;
code_algoritmo algoritmo;
int participantes_lock;


// SEMAFOROS (pthread_mutex_t Y sem_t)
pthread_mutex_t mutexIdProceso;
pthread_mutex_t mutexGradoMultiprogramacion;
pthread_mutex_t mutex_page_fault;
sem_t pendientes_new;
sem_t pendientes_ready;
sem_t pendientes_unlock;
sem_t pendientes_exit;
sem_t sem_multiprogramacion;
sem_t sem_quantum;
sem_t cpu_libre;
sem_t detenerPlanificacion;
sem_t nuevos_new;

// MENSAJES CONSOLA:
typedef enum code_mensaje
{
	MSJ_DESCONOCIDO= -1,
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	DETENER_PLANIFICACION,
	INICIAR_PLANIFICACION,
	MULTIPROGRAMACION,
	PROCESO_ESTADO,
} code_mensaje;

// FUNCIONES
void logear_config();
void liberar_programa();


#endif /* KERNEL_H_ */
