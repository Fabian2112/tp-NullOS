/*
 ============================================================================
 Name        : memoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "memoria.h"
/*
t_list * ListaInstrucciones(){

	t_list *testInstr = list_create();
	    list_add(testInstr, 0);
	    list_add(testInstr, 22);
	    list_add(testInstr, 32);
	    list_add(testInstr, 33);

	  return testInstr ;
}
t_list * ListaInstrucciones2(){

	t_list *testInstr = list_create();
	    list_add(testInstr, 0);
	    list_add(testInstr, 22);
	    list_add(testInstr, 32);
	    list_add(testInstr, 33);

	  return testInstr ;
}
*/
#define CONFIG_FILE_PATH "memoria.config"
#define LOG_FILE_PATH "memoria.log"

int main(void) {

	logger= LogInit(LOG_FILE_PATH, "MEMORIA", true);
	config = config_create(CONFIG_FILE_PATH);

	    if (config == NULL)
	    {
	        log_error(logger, "No se creó el archivo: %s", CONFIG_FILE_PATH);
	        log_destroy(logger);
	        return EXIT_FAILURE;
	    }

	   log_info(logger, "INICIANDO MODULO MEMORIA");

	   /*Datos de config*/
	   log_info(logger, "PUERTO_ESCUCHA: %s \n", puerto_escuha());
	   log_info(logger, "IP_FILESYSTEM: %s \n", ip_filesystem());
	   log_info(logger, "PUERTO_FILESYSTEM: %s \n",puerto_filesystem());
	   log_info(logger, "TAM_MEMORIA: %d \n",tam_memoria());
	   log_info(logger, "TAM_PAGINA: %d \n", tam_pagina());
	   log_info(logger, "PATH_INSTRUCCIONES: %s \n", path_instrucciones());
	   log_info(logger, "RETARDO_RESPUESTA: %d \n", retardo_respuesta());
	   log_info(logger, "ALGORITMO_REEMPLAZO: %s \n", algoritmo_reemplazo());

	   inicializar_memoria_instrucciones();
	   inicializar_estructuras();


/*	//test
	   t_list* proces1 =send_init_proceso_fs(64);
	   send_fin_proceso_fs(proces1);
	   t_list* proces2 =send_init_proceso_fs(32);

	   //test swap out
	     void* valor_enviar = malloc(tam_pagina() );
         int valor= 170;
	     memcpy(valor_enviar, &valor, tam_pagina());
	     int nro_bloque = 0;

		int socket_swap_out = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
		t_paquete* paquete = crear_paquete(_SWAP_OUT); // OP_CODE: SWAP_OUT
		agregar_a_paquete(paquete, valor_enviar, tam_pagina() );
		agregar_a_paquete(paquete, &nro_bloque, sizeof(int));

    	enviar_paquete(paquete, socket_swap_out);

        	send_swap_in(0);
  */


	   iniciar_conexiones();


	   ConfigTerminate();
	   liberar_estructuras_instrucciones();
	   liberar_estructuras();


	   log_info(logger, "FINALIZANDO MODULO MEMORIA");
	   LogTerminate(logger);
	   return EXIT_SUCCESS;
}

