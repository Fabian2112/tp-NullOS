/*
 * acciones.h
 *
 *  Created on: Nov 29, 2023
 *      Author: utnso
 */

#ifndef ACCIONES_H_
#define ACCIONES_H_

#include <shared_utils.h>
#include "datos_config.h"
#include "estructuras.h"
#include "conexion_filesystem.h"

typedef struct {
	int nro_bloque;
	uint32_t valor_bloque;
} t_block_file;


extern uint32_t* tablaFat;   //archivo fat
extern void* bloques_datos;

//Peticiones Kernel
//ABRIR
void abrir_archivo(char* nombre_archivo);
void truncar_archivo(char* nombre_archivo, int tamanio_nuevo);
//TRUNCAR
void truncar_desde_un_bloque(t_fcb* fcb, int bloques_a_asignar, int puntero_inicial);
void ampliar_tamanio(t_fcb* fcb, int bloques_a_asignar);
void reducir_tamanio(t_fcb* fcb, int bloques_a_asignar);

//WRITE
void peticion_escritura(char* nombre_archivo,int puntero, int direccion_fisica, int pid );
void write_file(void* valor_a_escribir, int nro_bloque );
void* pedir_valor_escritura(int pid, int direccion_fisica);

//WRITE
void peticion_lectura(char* nombre_archivo,int puntero, int direccion_fisica, int pid );
void enviar_memoria_read(int pid, int direccion_fisica, void* valor);




t_list* cargar_entradas_archivo(t_fcb* fcb);



#endif /* ACCIONES_H_ */
