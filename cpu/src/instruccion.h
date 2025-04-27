/*
 * instruccion.h
 *
 *  Created on: Sep 28, 2023
 *      Author: utnso
 */

#ifndef INSTRUCCION_H_
#define INSTRUCCION_H_

#include <shared_utils.h>
#include "conexion_cpu.h"

extern t_log* logger;
extern bool llego_interrupcion;
extern int socket_dispatch;
extern int socket_interrupt;

extern op_code codigo_interrupcion;

void cicloInstruccion(t_contexto *contexto_ejecucion);
t_instruccion* fetch(t_contexto *contexto_ejecucion);
void decode(t_instruccion* instruccion_actual, t_contexto* contexto_ejecucion);
void exec(t_instruccion* instruccion, t_contexto *contexto_ejecucion);

t_instruccion* solicitarInstruccion(int pid, int program_counter); // Solicitar a memoria la siguiente instrucción
void atender_interrupcion(t_contexto* contexto_ejecucion); // Si hay interrupción, dejar el PC en -1 ??, falta terminar función

//Funciones de ejecución de instrucciones
void enviar_contexto_un_valor(t_contexto* contexto_ejecucion, int op_code, char* valor);
void enviar_contexto_dos_valores(t_contexto* contexto_ejecucion, int op_code, char* valor1, char* valor2);

//MMU- traducciones de direcciones lógicas a físicas
void realizar_traduccion(int direccion_logica, int pid );

//READ/REWRITE en Memoria
int obtener_valor_memoria(int pid, int tamanio_lectura);
void escribir_valor_memoria(int pid, uint32_t valor, int tamaño_escritura);

int solicitar_tamanio_pagina();

#endif /* INSTRUCCION_H_ */
