/*
 * instruccion.c
 *
 *  Created on: Sep 28, 2023
 *      Author: utnso
 */
#include "instruccion.h"

int nro_marco;
int numero_pagina;
int direccion_fisica;
int tamanio_pagina;
int tamaño_registro = sizeof(uint32_t);

/*
t_list * ListaInstrucciones(){

	t_list *testInstr = list_create();
	    list_add(testInstr, "SET AX 1000");  //pc = 0
	    list_add(testInstr, "JNZ AX 3");
	    list_add(testInstr, "SET BX 2222");  //no debería ejecutar
	    list_add(testInstr, "SET AX 1");     //arranca acá
	    list_add(testInstr, "JNZ BX 3");   //ignorar cambio de pc
	    list_add(testInstr, "SET BX 2");
	    list_add(testInstr, "SET CX 3");
	    list_add(testInstr, "SET DX 4");
	    list_add(testInstr, "SUM AX BX");
	    list_add(testInstr, "SUB DX CX");
	    //list_add(testInstr, "SLEEP 5");   //SOLO PARA PROBAR SLEEP POR QUE FINALIZA EJECUCION
	   // list_add(testInstr, "WAIT RA");   //SOLO PARA PROBAR WAIT POR QUE FINALIZA EJECUCION
	    //list_add(testInstr, "SIGNAL RA");   //SOLO PARA PROBAR SIGNAL POR QUE FINALIZA EJECUCION
	    //list_add(testInstr, "MOV_IN DX 2");
	    //list_add(testInstr, "MOV_OUT 2 DX");
	    //list_add(testInstr, "F_OPEN ARCHIVO W");  //finaliza la ejecucion
	    //list_add(testInstr, "F_CLOSE ARCHIVO");   //FINALIZA LA EJECUCION
	   //list_add(testInstr, "F_SEEK ARCHIVO 2");  //finaliza la ejecucion
	    //list_add(testInstr, "F_READ ARCHIVO 2");  //finaliza la ejecucion
	    //list_add(testInstr, "F_WRITE ARCHIVO 2");  //finaliza la ejecucion
	    //list_add(testInstr, "F_TRUNCATE ARCHIVO 2");  //finaliza la ejecucion
	    list_add(testInstr, "EXIT");
	  return testInstr ;

	}
*/

void cicloInstruccion(t_contexto *contexto_ejecucion){
	t_instruccion* instruccion_actual ;

	while(contexto_ejecucion->pid != 0){

	//fetch
	instruccion_actual = fetch(contexto_ejecucion);

    //decode
	if (instruccion_actual != NULL){
		decode(instruccion_actual, contexto_ejecucion);
	}

	//exec
	exec(instruccion_actual, contexto_ejecucion);

	 liberarParametros(instruccion_actual->parametros); //free(instruccion_actual->parametros);
	 free(instruccion_actual);

	//atender instrucción si los hay
    if(llego_interrupcion && contexto_ejecucion->pid != 0){
			atender_interrupcion(contexto_ejecucion);
		}
     llego_interrupcion = false;

	}
	 log_info(logger, "Valor final del registro AX < %d > ", obtenerValorRegistro("AX", contexto_ejecucion->registros) );
	 log_info(logger, "Valor final del registro BX < %d > ", obtenerValorRegistro("BX", contexto_ejecucion->registros) );
	 log_info(logger, "Valor final del registro CX < %d > ", obtenerValorRegistro("CX", contexto_ejecucion->registros) );
	 log_info(logger, "Valor final del registro DX < %d > ", obtenerValorRegistro("DX", contexto_ejecucion->registros) );

	liberarContexto(contexto_ejecucion);
}

t_instruccion* fetch(t_contexto* contexto_ejecucion){
	int pid = contexto_ejecucion->pid;
	int program_counter = contexto_ejecucion->program_counter;

	t_instruccion* instruccion;

	log_info(logger, "PID: < %d > - FETCH - Program Counter: < %d > ", pid, program_counter);

	instruccion = solicitarInstruccion(pid, program_counter); // Solicitar a memoria la siguiente instrucción ( PC ,PID)

	contexto_ejecucion->program_counter++;

	return instruccion;

}

void decode(t_instruccion* instruccion_actual, t_contexto* contexto_ejecucion){
    if (instruccion_actual->instruccion == MOV_IN || instruccion_actual->instruccion == F_READ || instruccion_actual->instruccion == F_WRITE ) {
    	log_info(logger,"primer param %s, segundo %s",instruccion_actual->parametros[1], instruccion_actual->parametros[2] );
    	realizar_traduccion(atoi(instruccion_actual->parametros[2]) , contexto_ejecucion->pid );  //direccionLógica
    }
    if (instruccion_actual->instruccion == MOV_OUT ){
    	realizar_traduccion(atoi(instruccion_actual->parametros[1]), contexto_ejecucion->pid );  //direccionLógica
    }

}

void exec(t_instruccion* instruccion,t_contexto* contexto_ejecucion){
	uint32_t valor_destino;
	uint32_t valor_origen;
	uint32_t valor;
	char* registro;
	t_registros* registros;

	switch(instruccion->instruccion){
			case SET:
				registro = instruccion->parametros[1];
				valor = atoi(instruccion->parametros[2]);
				registros = contexto_ejecucion->registros;

				asignarValorRegistro(registro, valor,registros );
				log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s %d >  ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), registro, valor );

				break;

			case SUM:
				registro = instruccion->parametros[1];
				valor_destino= obtenerValorRegistro(instruccion->parametros[1],contexto_ejecucion->registros);
			    valor_origen = obtenerValorRegistro(instruccion->parametros[2],contexto_ejecucion->registros);
			    valor = (valor_destino + valor_origen);
			    registros = contexto_ejecucion->registros;

			    asignarValorRegistro(registro ,valor , registros);
			    log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %d %d > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), valor_destino , valor_origen );
				break;

			case SUB:
				registro = instruccion->parametros[1];
				valor_destino= obtenerValorRegistro(instruccion->parametros[1],contexto_ejecucion->registros);
			    valor_origen = obtenerValorRegistro(instruccion->parametros[2],contexto_ejecucion->registros);
			    valor = (valor_destino - valor_origen);
			    registros = contexto_ejecucion->registros;

			    asignarValorRegistro(registro ,valor , registros);
			    log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %d %d > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), valor_destino , valor_origen );
			    break;

			case JNZ:
				valor = obtenerValorRegistro(instruccion->parametros[1],contexto_ejecucion->registros);
				if(valor != 0){
					contexto_ejecucion->program_counter = atoi(instruccion->parametros[2]);
				};
				log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s %d > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1] , atoi(instruccion->parametros[2]) );
				break;

			case SLEEP:
				log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %d > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), atoi(instruccion->parametros[1]) );
				//enviar_contexto_un_valor(contexto_ejecucion, _SLEEP,  instruccion->parametros[1] );
				enviarContexto(contexto_ejecucion,_SLEEP,socket_dispatch);
				enviar_mensaje(instruccion->parametros[1], socket_dispatch);

				contexto_ejecucion->pid  = 0;
				break;

			case WAIT:
				log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1] );
				enviarContexto(contexto_ejecucion,_WAIT,socket_dispatch);
				enviar_mensaje(instruccion->parametros[1], socket_dispatch);

				contexto_ejecucion->pid  = 0;
				break;

			case SIGNAL:
				log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1] );
				enviarContexto(contexto_ejecucion,_SIGNAL,socket_dispatch);
				enviar_mensaje(instruccion->parametros[1], socket_dispatch);

				contexto_ejecucion->pid  = 0;
				break;
			case MOV_IN:
				log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s %d > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1] , atoi(instruccion->parametros[2]) );
				if(nro_marco < 0)
				{
					log_info(logger, "Page Fault PID: < %d > - Página: < %d >", contexto_ejecucion->pid, numero_pagina );
					contexto_ejecucion->program_counter--; log_info(logger, "Valor decrementado del program_counter   %d" , contexto_ejecucion->program_counter);
					enviarContexto(contexto_ejecucion,_PAGE_FAULT, socket_dispatch);
					char* nro_pagina_mov_in = string_itoa(numero_pagina);
					enviar_mensaje(nro_pagina_mov_in, socket_dispatch);
					free(nro_pagina_mov_in);
					contexto_ejecucion->pid  = 0;
					break;
				}
				registro = instruccion->parametros[1];
				registros = contexto_ejecucion->registros;
				valor = obtener_valor_memoria(contexto_ejecucion->pid, tamaño_registro);
				asignarValorRegistro(registro, valor, registros);
				break;

            case MOV_OUT:
				log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %d %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion),  atoi(instruccion->parametros[1]) , instruccion->parametros[2] );
				if(nro_marco < 0)
				{
					log_info(logger, "Page Fault PID: < %d > - Página: < %d >", contexto_ejecucion->pid, numero_pagina);
					contexto_ejecucion->program_counter--;
					log_info(logger, "Valor decrementado del program_counter   %d" , contexto_ejecucion->program_counter);
					enviarContexto(contexto_ejecucion,_PAGE_FAULT, socket_dispatch);
					char* nro_pagina_mov_out = string_itoa(numero_pagina);
					enviar_mensaje(nro_pagina_mov_out, socket_dispatch);
					free(nro_pagina_mov_out);
					contexto_ejecucion->pid  = 0;
					break;
				}
				registro = instruccion->parametros[2];
				registros = contexto_ejecucion->registros;
				valor = obtenerValorRegistro(registro, registros);
				escribir_valor_memoria(contexto_ejecucion->pid, valor, tamaño_registro);
				break;

            case F_OPEN:
             	log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1],instruccion->parametros[2] );
     			enviarContexto(contexto_ejecucion,_OPEN,socket_dispatch);
     		    enviar_mensaje(instruccion->parametros[1], socket_dispatch);
     		    enviar_mensaje(instruccion->parametros[2], socket_dispatch);

            	   contexto_ejecucion->pid  = 0;
            	   break;
            case F_CLOSE:
            	log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1] );
            	enviarContexto(contexto_ejecucion,_CLOSE,socket_dispatch);
            	enviar_mensaje(instruccion->parametros[1], socket_dispatch);

            	    contexto_ejecucion->pid  = 0;
            	    break;
			case F_SEEK:
             	log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1],instruccion->parametros[2] );
     			enviarContexto(contexto_ejecucion,_SEEK,socket_dispatch);
     		    enviar_mensaje(instruccion->parametros[1], socket_dispatch);
     		    enviar_mensaje(instruccion->parametros[2], socket_dispatch);

            	   contexto_ejecucion->pid  = 0;
                   break;

           case F_READ:
            	log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1],instruccion->parametros[2] );
            	if(nro_marco < 0)
            	{
					log_info(logger, "Page Fault PID: < %d > - Página: < %d >", contexto_ejecucion->pid, numero_pagina );
					contexto_ejecucion->program_counter--; log_info(logger, "Valor decrementado del program_counter   %d" , contexto_ejecucion->program_counter);
					enviarContexto(contexto_ejecucion,_PAGE_FAULT, socket_dispatch);
					char* nro_pagina_read = string_itoa(numero_pagina);
					enviar_mensaje(nro_pagina_read, socket_dispatch);
					free(nro_pagina_read);
            		contexto_ejecucion->pid = 0;
					break;
            	}
            	enviarContexto(contexto_ejecucion,_READ,socket_dispatch);
            	enviar_mensaje(instruccion->parametros[1], socket_dispatch);

            	char* dir_fisica_read = string_itoa(direccion_fisica);
            	enviar_mensaje(dir_fisica_read, socket_dispatch);
            	free(dir_fisica_read);

           	    contexto_ejecucion->pid  = 0;
                break;

			case F_WRITE:
            	log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1],instruccion->parametros[2] );
            	if(nro_marco < 0)
            	{
					log_info(logger, "Page Fault PID: < %d > - Página: < %d >", contexto_ejecucion->pid, numero_pagina );
					contexto_ejecucion->program_counter--; log_info(logger, "Valor decrementado del program_counter   %d" , contexto_ejecucion->program_counter);
					enviarContexto(contexto_ejecucion,_PAGE_FAULT, socket_dispatch);
					char* nro_pagina_write = string_itoa(numero_pagina);
					enviar_mensaje(nro_pagina_write, socket_dispatch);
					free(nro_pagina_write);
            		contexto_ejecucion->pid  = 0;
					break;
            	}
            	enviarContexto(contexto_ejecucion,_WRITE,socket_dispatch);
            	enviar_mensaje(instruccion->parametros[1], socket_dispatch);

            	char* dir_fisica_write = string_itoa(direccion_fisica);
            	enviar_mensaje(dir_fisica_write, socket_dispatch);
            	free(dir_fisica_write);

           	    contexto_ejecucion->pid  = 0;
                break;

			case F_TRUNCATE:
                log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s %s %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion), instruccion->parametros[1],instruccion->parametros[2] );
     			enviarContexto(contexto_ejecucion,_TRUNCATE,socket_dispatch);
     		    enviar_mensaje(instruccion->parametros[1], socket_dispatch);
     		    enviar_mensaje(instruccion->parametros[2], socket_dispatch);

     			contexto_ejecucion->pid  = 0;
                break;

				case EXIT:
				log_info(logger, "PID: < %d > - EXEC -Instruccion: < %s > ", contexto_ejecucion->pid, instruccionToString(instruccion->instruccion) );

				 enviarContexto(contexto_ejecucion,_EXIT, socket_dispatch);
				    contexto_ejecucion->pid  = 0;
				   break;

			    default:
			        break;
		}
}


t_instruccion* solicitarInstruccion(int pid, int program_counter){
	t_instruccion* instruccion = malloc(sizeof(t_instruccion));

	//Solicitar sgte instruccion

	 t_peticion_instruccion* pedir_instruccion =   malloc (sizeof(t_peticion_instruccion));
	 pedir_instruccion->pid= pid;
	 pedir_instruccion->program_counter = program_counter;
	 t_paquete* paquete= crear_paquete(_PEDIR_INSTRUCCION);

	 serializar_peticion_instruccion(paquete, pedir_instruccion);
     enviar_paquete(paquete, socket_memoria);
     free(pedir_instruccion);

     char* mensaje= recibir_valor(socket_memoria);
     //printf("Me llego el mensaje %s", mensaje);

	 /*	//Borrar solo test
	 t_list *testInstr = ListaInstrucciones();
	 char* mensaje= list_get(testInstr, program_counter);
   //Borrar solo para test
      */

	 uint8_t instruccion_code = obtenerInstruccion(mensaje);
     uint8_t cant_parametros = obtenerCantidadParametros(instruccion_code);
     char** parametros = obtenerParametros(mensaje, cant_parametros);

     instruccion->instruccion = instruccion_code ;
     instruccion->cant_parametros = cant_parametros ;
     instruccion->parametros= parametros;

     free(mensaje);

     return instruccion;
}


void atender_interrupcion(t_contexto* contexto_ejecucion){
	op_code codigo = _DESALOJO ;

	if (codigo_interrupcion == _FINALIZAR_PROCESO ){  //motivo interrupción por fin de proceso
		codigo = _EXIT;
	}

     enviarContexto(contexto_ejecucion, codigo ,socket_dispatch);
	 log_info(logger,"Se desaloja el PID: %d" , contexto_ejecucion->pid);

	 contexto_ejecucion->pid  = 0;
	 llego_interrupcion = false;

}

void enviar_contexto_un_valor(t_contexto* contexto_ejecucion, int op_code, char* valor){
	int tamanio_valor = strlen(valor) + 1;
	t_paquete* paquete = crear_paquete(op_code);
	     serializar_contexto(paquete, contexto_ejecucion);
	     agregar_a_paquete(paquete, &tamanio_valor, sizeof(int));
	     agregar_a_paquete(paquete, valor, tamanio_valor);
		 enviar_paquete(paquete,socket_dispatch);
}

void enviar_contexto_dos_valores(t_contexto* contexto_ejecucion, int op_code, char* valor1, char* valor2){
	t_paquete* paquete = crear_paquete(op_code);
	     serializar_contexto(paquete, contexto_ejecucion);
	     agregar_a_paquete(paquete, &valor1, strlen(valor1)+1 );
	     agregar_a_paquete(paquete, &valor2, strlen(valor2)+1 );
		 enviar_paquete(paquete,socket_dispatch);
}



void realizar_traduccion(int direccion_logica, int pid ){

    nro_marco = 0;	// seteo el valor de marco antes de realizar traduccion

   // verifico si ya fue consultado el tam. de pagina a memoria
    if(tamanio_pagina <= 0){
    	tamanio_pagina= solicitar_tamanio_pagina();
    }

    numero_pagina = floor( direccion_logica / tamanio_pagina );
    int desplazamiento = direccion_logica - numero_pagina * tamanio_pagina;
    log_info(logger, "Nro pagina: %d | desplazamiento: %d ", numero_pagina, desplazamiento);

    // SOLICITAR MARCO
    t_paquete* paquete= crear_paquete(_NRO_MARCO);
    agregar_a_paquete(paquete, &numero_pagina, sizeof(int) );
    agregar_a_paquete(paquete, &pid, sizeof(int) );
    enviar_paquete(paquete, socket_memoria);

    // RECIBIR MARCO
    char *mensaje = recibir_valor(socket_memoria);
    nro_marco = atoi(mensaje);
    free(mensaje);

    if (nro_marco >= 0){
    	log_info(logger, "PID: %d -OBTUVE MARCO - Página:< %d  > - Marco: < %d >", pid, numero_pagina, nro_marco);
        direccion_fisica = (nro_marco * tamanio_pagina) + desplazamiento;
    }
}


int obtener_valor_memoria(int pid, int tamanio_lectura){
	t_paquete* paquete = crear_paquete(_MOV_IN);
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, &tamanio_lectura, sizeof(int));
	enviar_paquete(paquete, socket_memoria);

	//RECIBIR VALOR
	//char *mensaje = recibir_valor(socket_memoria);
	//printf("El valor de mov_in es %s", mensaje);
	recibir_operacion(socket_memoria);
	t_list* mov_in_params = recibir_paquete(socket_memoria);
	int* valor = list_get(mov_in_params, 0);

    //int valor = atoi(mensaje);
    //free(mensaje);
	return *valor;
}


void escribir_valor_memoria(int pid, uint32_t valor, int tamanio_escritura){
	 char* valor_escritura = string_itoa(valor);
	 int tamanio_valor = strlen(valor_escritura) + 1;


	 t_paquete* paquete= crear_paquete(_MOV_OUT);
	 agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
     //agregar_a_paquete(paquete, valor_escritura, tamanio_valor);
	 agregar_a_paquete(paquete, &valor, sizeof(uint32_t));
     agregar_a_paquete(paquete, &tamanio_escritura, sizeof(int));
     agregar_a_paquete(paquete, &pid, sizeof(int));
	 enviar_paquete(paquete, socket_memoria);

	 free(valor_escritura);
}

int solicitar_tamanio_pagina(){
	int valor = 0;
	t_paquete* paquete = crear_paquete(_TAMANIO_PAGINA);
	agregar_a_paquete(paquete, &valor, sizeof(int) );
	enviar_paquete(paquete, socket_memoria);

	//RECIBIR EL TAMANIO
	char* mensaje = recibir_valor(socket_memoria);
	log_info(logger,"Tamanio de pagina %s", mensaje);
	int tam_pagina = atoi(mensaje);
	free(mensaje);
	return tam_pagina;

}
