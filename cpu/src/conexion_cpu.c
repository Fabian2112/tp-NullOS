/*
 * conexion_cpu.c
 *
 *  Created on: Sep 17, 2023
 *      Author: utnso
 */
#include "conexion_cpu.h"

pthread_t thread_dispatch;
pthread_t thread_interrupt;


t_contexto *inicializarContextotest()
{
  //BORRAR - SOLO TEST
  t_contexto *testContexto = malloc(sizeof(t_contexto));
 // t_registros *testRegistros= malloc(sizeof(t_registros));
   testContexto->pid = 1;
   testContexto->program_counter = 0; //INICIAL
      return testContexto;
}


int iniciar_conexiones(){
	socket_memoria = crear_conexion(ip_memoria(), puerto_memoria());


	    if (socket_memoria < 0)
	    {
	    	 log_error(logger,"No se pudo conectar con Memoria");
	         return EXIT_FAILURE;
	    }
	    log_info(logger, "El socket conectado es: %d", socket_memoria);
	   int result =  handshakeCliente(socket_memoria, CPU);

	   if (result < 0 ){
		   log_error(logger,"Error en Handshake con Memoria");
		   	         return EXIT_FAILURE;
	   }

     int servidor_dispatch = iniciar_servidor(puerto_dispatch());
	 log_info(logger, "Servidor listo para recibir al cliente dispatch");

	 int servidor_interrupt = iniciar_servidor(puerto_interrupt());
	 log_info(logger, "Servidor listo para recibir al cliente interrupt");

	if (servidor_dispatch < 0 || servidor_interrupt < 0)
	   {
	     log_error(logger, "Error intentando iniciar Servidor CPU.");
	     return EXIT_FAILURE;
	   }

	 pthread_create(&thread_dispatch, NULL, (void *)esperar_dispatch, (void *)servidor_dispatch);
	 pthread_create(&thread_interrupt, NULL, (void *)esperar_interrupt, (void *)servidor_interrupt);


	 pthread_join(thread_dispatch, NULL);
	 pthread_join(thread_interrupt, NULL);


    return EXIT_SUCCESS;

}


void esperar_dispatch(int servidor_dispatch){

	   //while(true){
	   socket_dispatch= esperar_cliente(servidor_dispatch);
	   int result = handshakeServidor(socket_dispatch);
	   log_info(logger, "Se conectó:  %s Disptach al socket:  %d", op_ModuloToString(result), socket_dispatch);
	   //Si se recibe un contexto de ejecución iniciar ciclo_instrucción

	   	     while (true)
	   	       {

	   	         switch (recibir_operacion(socket_dispatch))
	   	         {
	   	         case _CONTEXTO:
	   	        	 log_info(logger, "Se recibió contexto de ejecución \n");
	   	        	 t_contexto* contexto = deserializar_contexto(socket_dispatch);
	   	        	 log_info(logger,"Ciclo de instruccion para el PID %d ", contexto->pid);
	   	        	 cicloInstruccion(contexto);
	   	        	 break;

	   			case -1:
	   				log_info(logger, "Se desconectó KERNEL Dispatch");
	   				return;

	   			default:
	   				log_error(logger, "Respuesta de Kernel dispatch desconocida");
	   				continue;
	   	         }


	   	       }


	  // }
}

void esperar_interrupt(int servidor_interrupt){

	t_contexto* contexto_recibido;
	//while(true){
		 socket_interrupt =esperar_cliente(servidor_interrupt);
		 int result = handshakeServidor(socket_interrupt);
				        log_info(logger, "Se conectó:  %s Interrupt al socket:  %d", op_ModuloToString(result), socket_interrupt);
	      while (true)
		    {
		        switch (recibir_operacion(socket_interrupt))
		         {
		   	         case _DESALOJO:
				   	        contexto_recibido = deserializar_contexto(socket_interrupt); log_info(logger, "Interrumpir Ciclo del PID: %d", contexto_recibido->pid);
						        // Si llega una interrupción
						        	llego_interrupcion = true;
						        	codigo_interrupcion = _DESALOJO;
				   	           break;

		   	         case _FINALIZAR_PROCESO:
				   	       contexto_recibido = deserializar_contexto(socket_interrupt); log_info(logger, "Finalizar Ciclo del PID: %d", contexto_recibido->pid);
						        // Si llega una interrupción
						        	llego_interrupcion = true;
						        	codigo_interrupcion = _FINALIZAR_PROCESO;
				   	           break;

		   			case -1:
		   				log_info(logger, "Se desconectó Kernel interrupt");
		   				return;

		   			default:
		   				log_error(logger, "Respuesta de Kernel interrupt desconocida");
		   				continue;

				   	         }


				   	       }


	          }

