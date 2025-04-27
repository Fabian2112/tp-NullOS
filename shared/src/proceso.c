/*
 * instruccion.c
 *
 *  Created on: Sep 27, 2023
 *      Author: utnso
 */

#include "proceso.h"


uint32_t obtenerValorRegistro(char* registro, t_registros* registros){
	uint32_t valor;

	if(string_equals_ignore_case(registro,"AX") )
		valor = (registros->AX);
	if(string_equals_ignore_case(registro,"BX") )
		valor= (registros->BX);
	if(string_equals_ignore_case(registro,"CX") )
		valor =  (registros->CX);
	if(string_equals_ignore_case(registro,"DX") )
		valor = (registros->DX);

	return valor;
}

void asignarValorRegistro(char* registro, uint32_t valor,  t_registros* registros){
	     if(string_equals_ignore_case(registro,"AX") )
			(registros->AX) = valor;
		if(string_equals_ignore_case(registro,"BX") )
			(registros->BX) = valor;
		if(string_equals_ignore_case(registro,"CX") )
			(registros->CX) = valor;
		if(string_equals_ignore_case(registro,"DX") )
			(registros->DX) = valor;
}

uint8_t obtenerInstruccion(char* linea_instruccion){
	char* instruccion;
		char** parseo = string_split(linea_instruccion, " ");

		if(parseo[0] == NULL)
		{
			instruccion =  NULL;
		}
		else
		{
			instruccion = strdup(parseo[0]);
		}

	     // Liberar memoria del Split
		liberarParametros(parseo);

		uint8_t tipo = tipoInstruccion(instruccion);
		free(instruccion);
		return tipo;
}


char** obtenerParametros(char* linea_instruccion, int cant_parametros){
	char** parametros = string_split(linea_instruccion, " ");
	    int posicion= 0;

		while(parametros[posicion] != NULL)
		{
			posicion = posicion + 1;
		}

		if(posicion == (cant_parametros + 1))
		{
			return parametros;
		}else
		{
			liberarParametros(parametros);

	    	return NULL;
		}
}

uint8_t tipoInstruccion(char* valor_leido){
	if(string_equals_ignore_case(valor_leido,"SET") )
		return SET;
	if(string_equals_ignore_case(valor_leido,"SUM") )
		return SUM;
	if(string_equals_ignore_case(valor_leido,"SUB") )
		return SUB;
	if(string_equals_ignore_case(valor_leido,"JNZ") )
		return JNZ;
	if(string_equals_ignore_case(valor_leido,"SLEEP") )
		return SLEEP;
	if(string_equals_ignore_case(valor_leido,"WAIT") )
		return WAIT ;
	if(string_equals_ignore_case(valor_leido,"SIGNAL") )
		return SIGNAL;
	if(string_equals_ignore_case(valor_leido,"MOV_IN") )
		return MOV_IN;
	if(string_equals_ignore_case(valor_leido,"MOV_OUT") )
		return MOV_OUT;
	if(string_equals_ignore_case(valor_leido,"F_OPEN") )
		return F_OPEN;
	if(string_equals_ignore_case(valor_leido,"F_CLOSE") )
		return F_CLOSE;
	if(string_equals_ignore_case(valor_leido,"F_SEEK") )
		return F_SEEK;
	if(string_equals_ignore_case(valor_leido,"F_READ") )
		return F_READ;
	if(string_equals_ignore_case(valor_leido,"F_WRITE") )
			return F_WRITE;
	if(string_equals_ignore_case(valor_leido,"F_TRUNCATE") )
			return F_TRUNCATE;
	if(string_equals_ignore_case(valor_leido,"EXIT") )
	  return EXIT;

	return DESCONOCIDO;
}


void liberarParametros(char** parametros){
	int posicion;

		for(posicion= 0; parametros[posicion] != NULL; posicion++)
		{
		 		free(parametros[posicion]);
		}

			 	free(parametros);
}


uint8_t obtenerCantidadParametros(uint8_t instruccion){
	uint8_t cantidad_parametros = 0;
    switch(instruccion){
    case SET:
    case SUM:
    case SUB:
    case JNZ:
    case MOV_IN:
    case MOV_OUT:
    case F_OPEN:
    case F_SEEK:
    case F_READ:
    case F_WRITE:
    case F_TRUNCATE:
       return cantidad_parametros = 2; break;

    case SLEEP:
    case WAIT:
    case SIGNAL:
    case F_CLOSE:
     	 return cantidad_parametros = 1; break;

    case EXIT:
    	break;

    }
    return cantidad_parametros;
}


const char *estadoToString(code_estado code){
	switch(code){
	case NUEVO:
		return "NEW"; break;
	case LISTO:
		return "READY"; break;
	case EJECUTANDO:
		return "EXEC"; break;
	case BLOQUEADO:
		return "BLOCKED"; break;
	case FINALIZADO:
		return "EXIT"; break;
	 default:
		return "ERROR"; break;
	}

}

const char *motivoToString(motive_code code){
	switch(code){
	case SUCCESS:
		return "SUCCESS"; break;
	case INVALID_RESOURCE:
		return "INVALID_RESOURCE"; break;
	case INVALID_WRITE:
		return "INVALID_WRITE"; break;
	 default:
		return "ERROR"; break;
	}

}

const char *instruccionToString(instruccion code){
	switch(code){
		case SET:
					return "SET"; break;
		case SUM:
					return "SUM"; break;
		case SUB:
					return "SUB"; break;
		case JNZ:
					return "JNZ"; break;
		case SLEEP:
					return "SLEEP"; break;
		case WAIT:
					return "WAIT"; break;
		case SIGNAL:
					return "SIGNAL"; break;
		case MOV_IN:
					return "MOV_IN"; break;
		case MOV_OUT:
					return "MOV_OUT"; break;
		case F_OPEN:
					return "F_OPEN"; break;
		case F_CLOSE:
					return "F_CLOSE"; break;
		case F_SEEK:
					return "F_SEEK"; break;
		case F_READ:
					return "F_READ"; break;
		case F_WRITE:
					return "F_WRITE"; break;
		case F_TRUNCATE:
					return "F_TRUNCATE"; break;
		case EXIT:
					return "EXIT"; break;
		default:
				    return "ERROR"; break;
	}
}

void liberarContexto(t_contexto* contexto_ejecucion){
	free(contexto_ejecucion->registros);
	free(contexto_ejecucion);
}

void liberarProcesoNuevo(t_memoria_instruccion* proceso_nuevo) {
	//free((void*)proceso_nuevo->nombre_archivo);
	free(proceso_nuevo);
}

