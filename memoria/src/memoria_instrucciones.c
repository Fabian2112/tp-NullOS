/*
 * memoria_instrucciones.c
 *
 *  Created on: Oct 27, 2023
 *      Author: utnso
 */
#include "memoria_instrucciones.h"

void inicializar_memoria_instrucciones(){
	 lista_procesos = list_create();
	  sem_init(&enviar_instruccion, 0, 1);
}


void liberar_estructuras_instrucciones(){
	list_destroy_and_destroy_elements(lista_procesos, &free);
	sem_destroy(&enviar_instruccion);
}


void agregar_proceso(int pid, char* nombre_archivo){
	t_proceso* proceso = malloc(sizeof(t_proceso));
	t_list* lista_instrucciones= list_create();

	bool instrucciones_cargadas;

	proceso->pid= pid;
	proceso->nombre_archivo= strdup(nombre_archivo);

	char *path = string_new();
    string_append(&path , path_instrucciones() );
    string_append(&path , "/" );
    string_append(&path , nombre_archivo );
    log_info(logger, "El path del archivo es: %s." , path);

    instrucciones_cargadas = cargar_instrucciones(path, lista_instrucciones);

    if(instrucciones_cargadas){
    	proceso->lista_instrucciones = lista_instrucciones;
    	list_add(lista_procesos, proceso );
    	enviar_mensaje("OK", socket_kernel); log_info(logger, "Se generó la estructura de instrucciones correctamente");
    }
    else{
    	enviar_mensaje("ERROR", socket_kernel); log_info(logger, "Error al generar estructura de instrucciones");
    }


    free(path);

}


void buscar_sgte_instruccion(int pid,int program_counter){
   //Busco por el id de proceso
	  bool es_pid(void *_proceso)
	  {
		t_proceso* proceso  = (t_proceso *)_proceso;
	    return proceso->pid == pid;
	  }

	  t_proceso* proceso = list_find(lista_procesos, &es_pid);

	  instruccion_a_enviar = list_get(proceso->lista_instrucciones,program_counter );

	  usleep(retardo_respuesta() * 1000); //RETARDO DE MEMORIA
      enviar_mensaje(instruccion_a_enviar ,socket_cpu);

      log_info(logger, "Se envía la siguiente instruccion: %s , al socket cpu:  %d" , instruccion_a_enviar, socket_cpu);

     // sem_post(&enviar_instruccion);


}

void eliminar_instrucciones_proceso(int pid){

	//Busco por el id de proceso
		  bool es_pid(void *_proceso)
		  {
			t_proceso* proceso  = (t_proceso *)_proceso;
		    return proceso->pid == pid;
		  }

		  t_proceso* proceso = list_find(lista_procesos, &es_pid);

		  if(proceso != NULL){
			 list_destroy_and_destroy_elements(proceso->lista_instrucciones, &free);
			 bool removido = list_remove_element(lista_procesos, proceso);
	  	     if (removido == false){
			  	    	 log_info(logger, "No se encontró estructura para remover");
			  	  }else{
			  	    		 log_info(logger, "Se removió correctamente la estructura de instrucciones");
			  	    	 }
		  }
}

bool cargar_instrucciones(char* path, t_list* lista)
{
    FILE* archivo;
    archivo = fopen(path,"r");  log_info(logger, "hizo el fopen");

    if(archivo ==NULL){
        log_info(logger, "Error abriendo archivo\n"); return false;
       }
    int size = 50*sizeof(char);
    int i=0;
    char* buff = malloc(size);
    while(fgets(buff, size, archivo)){
        char* aux = malloc(size);
        strcpy(aux,buff);
        aux = strtok(aux,"\n");
        list_add(lista,aux);
        log_info(logger, "Indice: %d  Instruccion: %s", i, list_get(lista,i));

        i++;
    }
    free(buff);
    fclose(archivo);
    log_info(logger,"Fin de lectura del archivo!\n");
    return true;

	}
