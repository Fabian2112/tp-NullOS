/*
 * conexiones_kernel.h
 *
 *  Created on: Sep 22, 2023
 *      Author: utnso
 */

#ifndef CONEXIONES_KERNEL_H_
#define CONEXIONES_KERNEL_H_

#include <shared_utils.h>
#include "planificacion.h"
#include "recursos.h"

extern t_config* config;
extern t_log* logger;

extern int participantes_lock;

// CONEXIONES:

extern int socket_memoria;
extern int socket_filesystem;
extern int socket_dispatch;
extern int socket_interrupt;


extern pthread_t conexion_memoria;
extern pthread_t conexion_filesystem;
extern pthread_t conexion_cpu_dispatch;
extern pthread_t conexion_cpu_interrupt;

// UTILES:

typedef struct {
	t_pcb* pcb;
	int tiempo;
} t_sleep_arg;

typedef struct
{
    t_pcb* pcb;
    int nro_pagina;
} t_fault_arg;

typedef struct
{
    t_pcb* pcb;
    int socket_peticion;
} t_peticion_fs_arg;


// FUNCIONES:

bool iniciar_conexiones();
void liberar_conexiones();
int conectar_con_servidor(char* ip, char* puerto, char* servidor);
void crear_hilos_de_conexion();
void esperar_hilos_de_conexion();
void mensaje_memoria(int socket_memoria);
void recibir_filesystem(int socket_filesystem);
void recibir_cpu_dispatch(int *socket_dispatch);
void enviar_cpu_interrupt(int socket_interrupt);
void atender_sleep(t_sleep_arg* args);
void atender_page_fault(t_fault_arg* arg);
void atender_fin_peticion_fs(t_peticion_fs_arg* args);
void send_f_read_fs(char* nombre_archivo, int puntero_seek, int direccion_fisica, int socket_peticion, int pid);
void send_f_write_fs(char* nombre_archivo, int puntero_seek, int direccion_fisica, int socket_peticion, int pid);
void send_f_truncate_fs(char* nombre_archivo, int nuevo_tamanio, int socket_peticion);
//Enviar peticiones
void enviar_peticion_open(char* nombre_archivo);


#endif /* CONEXIONES_KERNEL_H_ */
