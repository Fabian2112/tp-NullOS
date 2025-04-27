/*
 * memoria_instrucciones.h
 *
 *  Created on: Oct 27, 2023
 *      Author: utnso
 */

#ifndef MEMORIA_INSTRUCCIONES_H_
#define MEMORIA_INSTRUCCIONES_H_

#include <shared_utils.h>
#include "datos_config.h"

typedef struct {
	int pid;
	char *nombre_archivo;
	t_list* lista_instrucciones;
} t_proceso;

extern char* instruccion_a_enviar;
extern t_log* logger;

extern int socket_memoria;
extern int socket_cpu;
extern int socket_filesystem;
extern int socket_kernel;

extern t_list* lista_procesos;
extern sem_t enviar_instruccion;

void inicializar_memoria_instrucciones();
void liberar_estructuras_instrucciones();
void agregar_proceso(int pid, char* nombre_archivo);

void buscar_sgte_instruccion(int pid,int program_counter);
void eliminar_instrucciones_proceso(int pid);

//file
bool cargar_instrucciones(char* path, t_list* list);

#endif /* MEMORIA_INSTRUCCIONES_H_ */
