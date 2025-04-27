/*
 * estructuras.h
 *
 *  Created on: Nov 26, 2023
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <shared_utils.h>
#include "datos_config.h"


typedef struct {
    char* nombre_archivo;
    int tamanio;
    int bloque_inicial;
} t_fcb;

typedef struct {
	int nro_bloque;
	int ocupado;
} t_swap_block;


typedef struct {
    char* valor;
    uint32_t bloque ;
} t_bloques;

extern int file_dat;
extern int file_fat;

extern uint32_t entrada_fat;
extern uint32_t* tablaFat;   //archivo fat

extern t_list* lista_fcbs;
extern t_list* lista_swaps;

extern void* bloques_datos ;

extern t_log* logger;
extern t_config *config;


bool inicializar_estructuras();

//SWAP
void escribir_bloque_swap(void* contenido_a_escribir, uint32_t nro_bloque );
void* leer_bloque_swap(uint32_t nro_bloque );
void inicializar_bloque_swap(int nro_bloque);
void liberar_bloque_swap(int nro_bloque);
int siguiente_bloque_swap_libre();
void cargar_lista_swap();

//Bloque de datos
bool inicializar_tabla_datos();
void escribir_bloque_datos(void* valor, uint32_t nro_bloque );
void* leer_bloque_datos(uint32_t nro_bloque );


//TABLA FAT
bool inicializar_tabla_fat();
void agregar_entrada( uint32_t valor, uint32_t indice );
uint32_t buscar_primera_entrada_libre();
int cantidad_entradas_libres();


//FCB
bool cargar_fcbs();
void crear_fcb(char* nombre_archivo);
t_fcb*  buscar_fcb(char* nombre_archivo);
void modificar_fcb(t_fcb *modificado);
void liberarfcb(t_fcb* fcb);



#endif /* ESTRUCTURAS_H_ */
