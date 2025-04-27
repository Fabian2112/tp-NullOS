/*
 * consola.h
 *
 *  Created on: Oct 14, 2023
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <shared_utils.h>
#include<readline/readline.h>
#include <readline/history.h>
#include "planificacion.h"
#include "datos_config.h"

extern t_config* config;
extern t_log* logger;

extern sem_t detenerPlanificacion;
extern sem_t sem_multiprogramacion;
extern pthread_mutex_t mutexGradoMultiprogramacion;
extern int gradoMultiprogramacion;

extern t_monitor* colas_new;
extern t_monitor* cola_ready;
extern t_monitor* cola_exec;
extern t_monitor* cola_block;
extern t_monitor* cola_exit;

void iniciar_consola();
void mensaje(char* mensaje);

void iniciar_proceso(char* mensaje);
void finalizar_proceso(char* mensaje);
void detener_planificacion();
void iniciar_planificacion();
void multiprogramacion(char* mensaje);
void proceso_estado();
void imprimir_colas(t_monitor* monitor, code_estado estado);
//finalizar proceso
void finalizar_id(int pid);
t_pcb* esta_pcb_estado(t_monitor* monitor, int pid);  //busca el pcb y retorna el índice



char*  obtenerCodigo(char* linea_mensaje);
char* obtenerNombreArchivo(char* linea);

#endif /* CONSOLA_H_ */
