/*
 ============================================================================
 Name        : filesystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "filesystem.h"

#define CONFIG_FILE_PATH "filesystem.config"
#define LOG_FILE_PATH "filesystem.log"

int main(void) {
	logger = LogInit(LOG_FILE_PATH, "FILESYSTEM", true);
	config = config_create(CONFIG_FILE_PATH);

	if (config == NULL){
		log_error(logger, "No se creó el archivo: %s", CONFIG_FILE_PATH);
		log_destroy(logger);
		return EXIT_FAILURE;
	}
	log_info(logger, "INICIANDO MODULO FILESYSTEM");

	if(inicializar_estructuras() == true){

	/* //test list
		char* prueba= "prueba";
		abrir_archivo(prueba);
		//truncar_archivo(prueba, 16);
		//truncar_archivo(prueba, 32);
		truncar_archivo(prueba,64);
		void* test = malloc(tamanio_bloque() );
		test= 26;
		write_file(&test, 1);
		 void* lectura_bloque= leer_bloque_datos(1 );   //read
		    	 mem_hexdump(lectura_bloque, sizeof(tamanio_bloque() ));

*/
		/*void* valor1 = malloc(tamanio_bloque() );
		valor1= 123;
    	write_file(&valor1, 1 ); //free(valor1);

    	void* valor2 = malloc(tamanio_bloque() );
    	valor2= 8;
    	swap_out(&valor2, 1);  //free(test);

    	void* valor3 = malloc(tamanio_bloque() );
    	valor2= 8;
    	swap_out(&valor3, 0);  //free(test);

    	void* valor4 = malloc(tamanio_bloque() );
    	valor4= 10;
    	swap_out(&valor4, 5);  //free(test);


    	liberar_bloque_swap(0);
    	void* lectura = leer_bloque_swap(5); //swap_in
    	 mem_hexdump(lectura, sizeof(tamanio_bloque() ));

    	 void* lectura_bloque= leer_bloque_datos(1 );   //read
    	 mem_hexdump(lectura_bloque, sizeof(tamanio_bloque() ));
    	 */

		iniciar_conexiones();
	};

	close(file_dat);
	close(file_fat);
	ConfigTerminate();
	liberarEstructuras();
	log_info(logger, "FINALIZANDO MODULO FILESYSTEM");
	LogTerminate(logger);

	return EXIT_SUCCESS;
}


void liberarEstructuras(){
	//Listas
	list_destroy_and_destroy_elements(lista_fcbs,(void*)liberarfcb);
	list_destroy_and_destroy_elements(lista_swaps, &free);

}
