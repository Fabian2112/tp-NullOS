/*
 * instruccion.h
 *
 *  Created on: Sep 27, 2023
 *      Author: utnso
 */

#ifndef PROCESO_H_
#define PROCESO_H_

#include "TypeDef.h"
#include <commons/collections/list.h>
#include <bits/types.h>

//Estructuras
typedef enum {
NUEVO,
LISTO,
EJECUTANDO,
BLOQUEADO,
FINALIZADO
} code_estado;

typedef struct{
uint32_t AX;
uint32_t BX;
uint32_t CX;
uint32_t DX;
} t_registros;

typedef enum instruccion
{
DESCONOCIDO= -1,
SET,
SUM,
SUB,
JNZ,
SLEEP,
WAIT,
SIGNAL,
MOV_IN,
MOV_OUT,
F_OPEN,
F_CLOSE,
F_SEEK,
F_READ,
F_WRITE,
F_TRUNCATE,
EXIT
} instruccion;

typedef struct{
uint8_t instruccion;
uint8_t cant_parametros;
char** parametros;
} t_instruccion;

typedef struct {
int pid;
int program_counter;
t_registros* registros;
} t_contexto;   //Para enviar a CPU

typedef enum {
SUCCESS,
INVALID_RESOURCE,
INVALID_SIGNAL,
INVALID_WRITE,
} motive_code;  //motivos de finalización de un proceso


typedef struct {
	int prioridad;
	int size;
	t_contexto* contexto;
	t_list*  file_opens;
	code_estado estado;
	char* nombre_txt;
	motive_code motivo_fin;
} t_pcb;


typedef struct
{
	int pid;
	int program_counter;
} t_peticion_instruccion;  //solo para CPU-Memoria

typedef struct
{
	int pid;
	int nombre_length;
	char* nombre_archivo;
} t_memoria_instruccion;  //solo para Kernel-Memoria para proceso nuevo

//REGISTROS
uint32_t obtenerValorRegistro(char* registro, t_registros* registros);
void asignarValorRegistro(char* registro, uint32_t valor,  t_registros* registros);


//PARSER DE INSTRUCCIONES
uint8_t obtenerInstruccion(char* linea_instruccion);  //Se pasa la línea completa de instrucción y devuelve code de la instruccion a ejecutar
char** obtenerParametros(char* linea_instruccion, int cant_parametros); // Se pasa la línea completa de instrucción/mensaje y la cant. de parámetros esperado y devuelve los parámetros
uint8_t tipoInstruccion(char* valor_leido);  //Se pasa el primer string leído de la línea de instrucción y se devuelve el code de la instrucción
void liberarParametros(char** parametros);

//OBTENER CANT DE PARAMETROS POR CODE DE INSTRUCCION
uint8_t obtenerCantidadParametros(uint8_t instruccion);//Al pasar el code de la instruccion , devuelve la cant. de parámetros a esperar

//PARA IMPRIMIR CODE DE ENUM
const char *estadoToString(code_estado code);  //imprimir estado
const char *instruccionToString(instruccion code); //imprimir instrucción
const char *motivoToString(motive_code code);

//Contexto y PCB
void liberarContexto(t_contexto* contexto_ejecucion);

void liberarProcesoNuevo(t_memoria_instruccion* proceso_nuevo) ;

#endif /* PROCESO_H_ */
