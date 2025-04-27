/*
 * estructuras.h
 *
 *  Created on: Dec 3, 2023
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <shared_utils.h>
#include "memoria_instrucciones.h"

// ESTRUCTURAS ADMINISTRATIVAS:

typedef struct {	// PROCESO CARGADO EN MEMORIA
	int pid;
	char* nombre;
	int tamanio;
	t_list* tabla_de_paginas;	// LISTA DE t_entrada_tabla ==> INDEXADO POR PAGINAS
} t_proceso_memoria;


typedef struct {	// ENTRADA TABLA DE PAGINAS (PAGINA)
	int marco;
	bool bit_presencia;
	bool bit_modificado;
	int ultimo_acceso;
	int instante_carga;
	int bloque_SWAP;
} t_entrada_tabla;


typedef struct {
	bool ocupado;
} t_marco;


extern void* memoria_usuario;
extern t_list* procesos_en_memoria;	// LISTA DE PROCESOS EN MEMORIA: necesaria para acceder a una TP a partir de un PID.
extern t_list* marcos;	// BITMAP DE MARCOS

// UTILES PARA SUSTITUCION:

typedef struct {
	int pid;
	int nro_pagina;
	t_entrada_tabla* pagina;
} t_pagina_presente;

extern t_list* lista_paginas_presentes;

extern int instante_carga; // AUTOINCREMENTAL PARA TODOS LOS PROCESOS
extern int instante_acceso; // idem

// FUNCIONES:

t_entrada_tabla* buscar_pagina(int pid, int nro_pagina);
int buscar_marco(int nro_pagina, int pid);
t_proceso_memoria* buscar_proceso(int pid);
t_entrada_tabla* buscar_pagina_en_tabla(t_list* tabla_de_paginas, int nro_pagina);
bool memoria_llena();
t_pagina_presente* comparar_instante_carga(t_pagina_presente* pagina_1, t_pagina_presente* pagina_2);
t_pagina_presente* comparar_ultimo_acceso(t_pagina_presente* pagina_1, t_pagina_presente* pagina_2);
int buscar_marco_libre();
void actualizar_ultimo_acceso(int pid, int direccion_fisica);
void actualizar_bit_modificado(int pid, int direccion_fisica);
int buscar_nro_pagina(int pid, int direccion_fisica);
int buscar_pagina_por_marco(int pid, int nro_marco);
int transformer_pagina_a_bloque_swap(t_entrada_tabla* pagina);
void* buscar_contenido_pagina(int nro_marco);

void inicializar_estructuras();
t_list* crear_marcos();
void crear_proceso(int pid, char* nombre_proceso, int tamanio_proceso);
void actualizar_referencias_a_SWAP(t_list* tabla_de_paginas, t_list* lista_bloques_SWAP);
t_list* crear_tabla_de_paginas(int pid, int tamanio_proceso, int tamanio_pagina);
t_entrada_tabla* crear_pagina();
t_pagina_presente* crear_pagina_presente(int pid, int nro_pagina, t_entrada_tabla* nueva_pagina);

void liberar_estructuras();
void destruir_pagina_presente(t_pagina_presente* pagina);
void destruir_paginas_presentes(t_proceso_memoria* proceso);
void destruir_proceso(t_proceso_memoria* proceso);
void destruir_pagina(t_entrada_tabla* pagina);
void liberar_bloques_swap(t_list* tabla_de_paginas);

void atender_page_fault(int pid, int nro_pagina);
int seleccionar_pagina_victima(int pid, int nro_pagina);

t_list* send_init_proceso_fs(int tamanio_proceso);
void send_fin_proceso_fs(t_list* bloques_SWAP);
void* send_swap_in(int nro_bloque);
void send_swap_out(void* contenido_pagina, int nro_bloque);
int conectar_con_servidor(char* ip, char* puerto, char* servidor);

#endif /* ESTRUCTURAS_H_ */
