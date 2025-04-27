/*
 * planificacion.h
 *
 *  Created on: Oct 19, 2023
 *      Author: utnso
 */

#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <shared_utils.h>
#include <commons/collections/queue.h>
#include "conexion.h"
#include "recursos.h"
#include "datos_config.h"

extern t_config* config;
extern t_log* logger;

// COLAS DE PLANIFICACION (con monitores)
extern t_monitor* cola_general;
extern t_monitor* colas_new;
extern t_monitor* cola_ready;
extern t_monitor* cola_exec;
extern t_monitor* cola_unlock;
extern t_monitor* cola_block;
extern t_monitor* cola_exit;



// VARIABLES

extern int id_proceso; // CONTADOR DE PIDS
extern int gradoMultiprogramacion;
extern bool desalojo;
extern code_algoritmo algoritmo;

// SEMAFOROS (pthread_mutex_t Y sem_t)
extern pthread_mutex_t mutexIdProceso;
extern pthread_mutex_t mutexGradoMultiprogramacion;
extern pthread_mutex_t mutex_page_fault;
extern sem_t pendientes_new;
extern sem_t pendientes_ready;
extern sem_t pendientes_unlock;
extern sem_t pendientes_exit;
extern sem_t sem_multiprogramacion;
extern sem_t sem_quantum;
extern sem_t cpu_libre;
extern sem_t detenerPlanificacion;
extern sem_t nuevos_new;



// VARIABLES DE CONEXION

extern int socket_memoria;
extern int socket_filesystem;
extern int socket_dispatch;
extern int socket_interrupt;

// FUNCIONES

void inicializar_variables();
void setear_algoritmo();
void iniciar_estructuras();
void liberar_estructuras();
void agregarNEW(t_pcb *pcb);
void planificar();
void planificador_largo_plazo();
void new_a_ready_PCB();
void exit_PCB();
void transicion_exec_a_exit(t_pcb* pcb);
void planificador_corto_plazo();
void ready_a_exec_PCB();
t_pcb* elegir_proximo_proceso();
void esperar_quantum();
void dispatcher(t_pcb* pcb);
void block_a_ready_PCB();
void transicion_a_ready(t_pcb* pcb);
t_pcb *crearPCB(int size, int prioridad, char* nombre_archivo);
void destruirPCB(t_pcb* pcb);
void cambiar_estado(t_pcb* pcb, code_estado nuevo_estado);
void list_add_con_mutex(t_monitor* monitor, void* data);
void* list_remove_con_mutex(t_monitor* monitor);
t_pcb* comparar_prioridad(t_pcb* proceso_1, t_pcb* proceso_2);
//void memoria_instrucciones(char* nombre_archivo, int id_proceso);  //Enviar para la estructura de memoria de instrucciones

void list_remove_pcb_con_mutex(t_monitor* monitor, t_pcb* pcb_proceso);  //remover pcb de una cola
void exec_a_ready_PCB(t_pcb* pcb);


void sigueEnNew(); //validar si no fue finalizado

void pasar_a_exit(t_monitor* monitor, t_pcb* pcb);  //finaliza a exit por pedido de consola

void finalizar_new_exit(t_pcb* pcb);
void finalizar_ready_exit(t_pcb* pcb);
void finalizar_exec_exit(t_pcb* pcb);
void finalizar_block_exit(t_pcb* pcb);
void finalizar_unlock_exit(t_pcb* pcb);


void enviar_crear_proceso(t_pcb* pcb);
void enviar_eliminar_proceso(t_pcb* pcb); //eliminar estructura en memoria
bool enviar_memoria_instrucciones(t_pcb* pcb);  //envío a memoria_instrucciones

#endif /* PLANIFICACION_H_ */
