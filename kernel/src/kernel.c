/*
 ============================================================================
 Name        : kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Kernel
 ============================================================================
 */

#include "../include/kernel.h"


#define CONFIG_FILE_PATH "kernel.config"	// ALTERNATIVA: Usar los parametros del main (argc y argv) para parametrizar el archivo de configuracion
#define LOG_FILE_PATH "kernel.log"


int main(void) {

	logger = LogInit(LOG_FILE_PATH, "Kernel", true);

    config = config_create(CONFIG_FILE_PATH);
    if(config == NULL){
		log_error(logger, "No se creó el archivo: %s", CONFIG_FILE_PATH);
		log_destroy(logger);
		return EXIT_FAILURE;
	}
	log_info(logger, "INICIANDO MODULO KERNEL");

	// LOGEO DATOS DE CONFIG
	logear_config();

	// CREO CONEXIONES CONL LOS SERVIDORES MEMORIA, FILESYSTEM, CPU DISPATCH Y CPU INTERRUPT
	if(!iniciar_conexiones()){
		log_error(logger, "Error al iniciar conexiones");
		liberar_programa();
		return EXIT_FAILURE;
	}
	crear_hilos_de_conexion();
	esperar_hilos_de_conexion();

	inicializar_variables();
	iniciar_estructuras();	// COLAS Y SEMAFOROS DE PLANIFICACION
	inicializar_recursos(); // RECURSOS
	inicializar_tabla_archivos();

	planificar();

	iniciar_consola()  ; // CONSOLA INTERACTIVA --> EJECUTA EN MAIN

	log_info(logger, "FINALIZANDO MODULO KERNEL");

	liberar_programa();

	return EXIT_SUCCESS;
}


void logear_config(){
	/*log_info(logger, "IP_MEMORIA: %s \n", ip_memoria());
	log_info(logger, "PUERTO_MEMORIA: %s \n", puerto_memoria());
	log_info(logger, "IP_FILESYSTEM: %s \n", ip_filesystem());
	log_info(logger, "PUERTO_FILESYSTEM: %s \n", puerto_filesystem());
	log_info(logger, "IP_CPU: %s \n", ip_cpu());
	log_info(logger, "PUERTO_CPU_DISPATCH: %s \n", puerto_dispatch());
	log_info(logger, "PUERTO_CPU_INTERRUPT: %s \n", puerto_interrupt());
	log_info(logger, "ALGORITMO_PLANIFICACION: %s \n", algoritmo_planificacion());
	log_info(logger, "QUANTUM: %d \n", quantum());
	log_info(logger, "GRADO_MULTIPROGRAMACION_INI: %d \n", grado_multiprogramacion());
	log_info(logger, "RECURSO 1: %s \n", recursos()[0]);
	log_info(logger, "RECURSO 2: %s \n", recursos()[1]);
	log_info(logger, "RECURSO 3: %s \n", recursos()[2]);
	log_info(logger, "INSTANCIA_RA: %s \n", instancias_recursos()[0]);
	log_info(logger, "INSTANCIA_RB: %s \n", instancias_recursos()[1]);
	log_info(logger, "INSTANCIA_RC: %s \n", instancias_recursos()[2]);*/
}


void liberar_programa(){
	LogTerminate(logger);
	ConfigTerminate();
	liberar_estructuras();
	liberar_conexiones();
	liberar_recursos();
	liberar_tabla_archivos();
}


