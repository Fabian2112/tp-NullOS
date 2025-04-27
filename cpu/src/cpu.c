/*
 ============================================================================
 Name        : cpu.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "cpu.h"

#define CONFIG_FILE_PATH "cpu.config"
#define LOG_FILE_PATH "cpu.log"

int main(void) {

	logger= LogInit(LOG_FILE_PATH, "CPU", true);
	config = config_create(CONFIG_FILE_PATH);

	    if (config == NULL)
	    {
	        log_error(logger, "No se creó el archivo: %s", CONFIG_FILE_PATH);
	        log_destroy(logger);
	        return EXIT_FAILURE;
	    }

	   log_info(logger, "INICIANDO MODULO CPU");

	   /*Datos de config*/
	   log_info(logger, "IP_MEMORIA: %s \n", ip_memoria());
	   log_info(logger, "PUERTO_MEMORIA: %s \n", puerto_memoria());
	   log_info(logger, "PUERTO_ESCUCHA_DISPATCH: %s \n", puerto_dispatch());
	   log_info(logger, "PUERTO_ESCUCHA_INTERRUPT: %s \n", puerto_interrupt());


	    iniciar_conexiones();

	 /*//Solo para test --borrar
	    t_contexto *contexto_ejecucion = malloc(sizeof(t_contexto));

	    contexto_ejecucion = inicializarContextotest();
	   t_registros* registros = malloc(sizeof(t_registros));
	    contexto_ejecucion->registros =  registros ;
	    registros->AX =0;
	    registros->BX= 0;
	    registros->CX= 0;
	    registros->DX= 0;

	   cicloInstruccion(contexto_ejecucion);
	   //SOLO PARA TEST FIN
*/

	   ConfigTerminate();
	   log_info(logger, "FINALIZANDO MODULO CPU");
	   LogTerminate(logger);
	   liberar_conexion(socket_memoria);
	      return EXIT_SUCCESS;
}


