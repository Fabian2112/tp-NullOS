/*
 * recursos.h
 *
 *  Created on: Oct 20, 2023
 *      Author: utnso
 */

#ifndef RECURSOS_H_
#define RECURSOS_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "planificacion.h"
#include "conexiones_kernel.h"
#include "consola.h"

// RECURSOS:

typedef struct {
	int id;
	char* nombre;
	int instancias;
	t_monitor* cola_asignados;
	t_monitor* cola_block;
} t_recurso;

extern t_list* lista_recursos;


// ARCHIVOS:

extern t_list* tabla_archivos_abiertos; // CONTIENE t_archivo

typedef enum {
	READ,
	WRITE
} code_lock;

typedef struct{
	char* nombre;
	t_monitor* cola_block;	// SE VAN A GUARDAR t_peticion_archivo
	code_lock lock_activo;
	int contador_de_aperturas;
}t_archivo;

typedef struct{
	char* nombre;
	int puntero;
}t_archivo_proceso; // SE VAN A GUARDAR EN PCB->file_opens

typedef struct{
	t_list* pcbs;
	code_lock tipo;
}t_peticion_archivo;

extern int participantes_lock;

// FUNCIONES:

void inicializar_recursos();
bool recurso_existe(char* nombre);
t_recurso* get_recurso(char* nombre);
void liberar_recursos();
void liberar_recurso(void* recurso);
void atender_wait(t_pcb* pcb, char* recurso);
void atender_signal(t_pcb* pcb, char* recurso);
bool proceso_tiene_asignado(t_pcb* pcb, t_recurso* recurso);
bool proceso_esta_pidiendo(t_pcb* pcb, t_recurso* recurso);

void inicializar_tabla_archivos();
void liberar_tabla_archivos();
void atender_f_open(t_pcb* pcb, char* nombre_archivo, code_lock tipo_lock);
t_archivo* get_archivo_abierto(char* nombre);
bool archivo_esta_abierto(char* nombre);
code_lock tipoLock(char* tipo);
const char* lock_to_string(code_lock tipo);
t_archivo* crear_archivo(char* nombre_archivo);
void destruir_archivo(t_archivo* archivo);
t_archivo_proceso* crear_archivo_proceso(t_archivo* archivo);
t_archivo_proceso* get_archivo_proceso(t_list* file_opens, char* nombre);
t_peticion_archivo* crear_peticion_archivo(t_pcb* pcb, code_lock tipo_lock);
void destruir_archivo_proceso(t_archivo_proceso* archivo_proceso);
void destruir_peticion_archivo(t_peticion_archivo* peticion);
t_peticion_archivo* obtener_peticion_archivo(t_pcb* pcb, t_archivo* archivo);

void liberar_recursos_pcb(t_pcb* pcb);
void simular_f_close(t_pcb* pcb, t_archivo_proceso* archivo_proceso);
void simular_signal(t_recurso* recurso);

void detectar_deadlock();
void detectar_espera_circular(int procesos, int transiciones[procesos][procesos]);
void inicializar_peticiones(int procesos, int recursos, int peticiones[procesos][recursos]);
void inicializar_asignaciones(int recursos, int procesos, int asignaciones[recursos][procesos]);
void multiplicar_matrices(int recursos, int procesos, int transiciones[procesos][procesos], int asignaciones[recursos][procesos], int peticiones[procesos][recursos]);
void floydwarshall(int procesos, int transiciones[procesos][procesos]);
void logear_matriz(int filas, int columnas, int matriz[filas][columnas]);

int* string_to_int_array(char** array_de_strings);

#endif /* RECURSOS_H_ */
