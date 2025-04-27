/*
 * acciones.c
 *
 *  Created on: Nov 29, 2023
 *      Author: utnso
 */

#include "acciones.h"

#define FIN_DE_ARCHIVO UINT32_MAX



/* 	===============================================================================================================
					ABRIR ARCHIVO
	===============================================================================================================*/

void abrir_archivo(char* nombre_archivo){
	//valido si existe fcb
	t_fcb* fcb =  buscar_fcb(nombre_archivo);
	if(fcb == NULL){
		// si no existe lo creo
		crear_fcb( nombre_archivo);
	}
}

/* 	===============================================================================================================
					TRUNCATE
	===============================================================================================================*/

void truncar_archivo(char* nombre_archivo, int tamanio_nuevo){
	//CONSULTO TAMANIO FCB
	t_fcb* fcb = (t_fcb*) malloc(sizeof(t_fcb));

    fcb = buscar_fcb(nombre_archivo);

   if(fcb == NULL){
	   log_info(logger, "No se encontró el archivo a truncar");

	   return;
   }
    uint32_t puntero_inicial = fcb->bloque_inicial;

   //BUSCO CANTIDAD DE BLOQUES A ASIGNAR  TAMANIO_TRUNCAR /TAMANIO_BLOQUE  y chequeo que haya lugar a asignar
	int bloques_a_asignar = ceil( (tamanio_nuevo -  fcb->tamanio)  / tamanio_bloque() ) ; 	//log_info(logger, "Falta asignar %d bloques", bloques_a_asignar);

     if(cantidad_entradas_libres() < bloques_a_asignar){
    	 return;
     }

   //TRUNCAR DEPENDIENDO DEL CASO
	if (fcb->tamanio == tamanio_nuevo){  // Si es igual, no hacer nada
		return;
	}


	if ( fcb->tamanio == 0){   //Agregar entradas a la FAT

    // bloques_a_asignar = bloques_a_asignar; // + 1 ;
      //log_info(logger, "Se van a asignar %d bloques ", bloques_a_asignar);
	  puntero_inicial = buscar_primera_entrada_libre(); //log_info(logger,"Primer bloque FAT asignado: %d al archivo: %s", puntero_inicial, fcb->nombre_archivo);
      agregar_entrada(FIN_DE_ARCHIVO , puntero_inicial);
      bloques_a_asignar--;
      fcb->bloque_inicial= puntero_inicial;
      truncar_desde_un_bloque(fcb, bloques_a_asignar, puntero_inicial);
      //truncar_desde_tamanio_cero(fcb, bloques_a_asignar);
	};

	if(fcb->tamanio  < tamanio_nuevo && fcb->tamanio > 0){ //AMPLIAR
		 ampliar_tamanio(fcb, bloques_a_asignar );


   	   //log_info(logger, "ACCESO FAT- Se asignaron  %d bloques", (int)ceil(tamanio_nuevo/tamanio_bloque() )+ 1  );
	}

	if(fcb->tamanio > tamanio_nuevo){  //REDUCIR
		reducir_tamanio(fcb, bloques_a_asignar);
		log_info(logger, "ACCESO FAT- Se asignaron %d bloques ", (int)ceil(tamanio_nuevo/tamanio_bloque() )+ 1  );
	}

	//ACTUALIZAR FCB
	fcb->tamanio = tamanio_nuevo;
	fcb->bloque_inicial = puntero_inicial;

	modificar_fcb(fcb);

	log_info(logger, "ENTRADAS FAT DEL FCB");
	 t_list* entradas_finales =cargar_entradas_archivo(fcb);

	     list_destroy_and_destroy_elements(entradas_finales, &free);

	usleep(retardo_acceso_fat() * 1000);

}

void truncar_desde_un_bloque(t_fcb* fcb, int bloques_a_asignar, int puntero_inicial){
	      uint32_t bloque_siguiente = 0;
	      uint32_t bloque_actual = puntero_inicial;

	      while(bloques_a_asignar > 0){
	        //Si hay aún bloques a asignar, entonces agrego el bloque anterior
	    	 bloque_siguiente = buscar_primera_entrada_libre(); //log_info(logger,"Siguiente bloque FAT asignado: %d al archivo: %s", bloque_siguiente, fcb->nombre_archivo);
	    	  agregar_entrada(bloque_siguiente , bloque_actual);
	    	  agregar_entrada(FIN_DE_ARCHIVO , bloque_siguiente); //solo por que me devuelve nuevamente como siguiente entrada libre
	    	  bloque_actual = bloque_siguiente ;
	    	  bloques_a_asignar--;

	    	   if(bloques_a_asignar == 0){ //ULTIMO BLOQUE EOF
	    		  uint32_t valor_grabado;
	    	      memcpy(&valor_grabado,tablaFat + bloque_actual * sizeof(uint32_t) ,sizeof(uint32_t));
	    	          	      	//log_info(logger, "Se grabó fin de archivo en la entrada fat %d ", bloque_siguiente);
	    	   }
	      }


}

void  ampliar_tamanio(t_fcb* fcb, int bloques_a_asignar){
	//log_info(logger, "Falta asignar %d bloques", bloques_a_asignar);
     uint32_t bloque_actual = fcb->bloque_inicial;  // log_info(logger,"bLOQUE ACTUAL : %d", bloque_actual);
	 uint32_t valor_grabado= 0;

	 //Busco la cantidad de bloques a recorrer (tamanio_actual/tam_bloque)
	 int cant_bloques_anteriores= ceil( (fcb->tamanio)  / tamanio_bloque() ); //+ 1;

	 //obtengo valor del bloque EOF
	 for(int i= 0 ; i < cant_bloques_anteriores; i++){
		 memcpy(&valor_grabado,tablaFat + (bloque_actual * sizeof(uint32_t) ) ,sizeof(uint32_t));

	 	if(valor_grabado == FIN_DE_ARCHIVO){
	 	 //bloque_actual = valor_grabado ;
	 		//log_info(logger,"Valor de bloque EOF anterior : %d",bloque_actual);
	 	 break;
	  }
	 	bloque_actual = valor_grabado;

 	 }

	 truncar_desde_un_bloque(fcb,bloques_a_asignar, bloque_actual);

}

void  reducir_tamanio(t_fcb* fcb, int bloques_a_sacar){
	log_info(logger, "Se debe sacar %d bloques", bloques_a_sacar*-1 );
	t_block_file* datos ;
    uint32_t valor_en_fat = 0;  //inicializar bloques
     void* valor_en_bloque = NULL;

     log_info(logger, "ACCESO FAT - Entradas antes de reducir tamanio");
     t_list* lista_bloques = cargar_entradas_archivo(fcb);

	  int indice = list_size(lista_bloques) - 1;

	 for(int i= ((bloques_a_sacar * - 1)  + 1 ); i  > 0 ; i--){   //+1

		 datos = list_get(lista_bloques , indice ); log_info(logger, "inidice %d, i %d", indice, i);

		 if(i ==1){  //Ultimo bloque a reducir no se limpia el bloque de datos solo se marcar EOF
			 agregar_entrada(FIN_DE_ARCHIVO , datos->nro_bloque);
			 indice--;

		 }
		 else {
		//hay que acceder también al área del archivo de bloques para inicializar en 0
		  agregar_entrada(valor_en_fat , datos->nro_bloque);

		  //Escribir en el bloque de datos
		  valor_en_bloque = malloc(tamanio_bloque());
		  memset(valor_en_bloque, 0, tamanio_bloque());
		  escribir_bloque_datos(valor_en_bloque , datos->nro_bloque );
          free(valor_en_bloque);
           indice--;
			log_info(logger, "Acceso Bloque - Archivo:%s - Bloque Archivo: %d - Bloque FS: %d", fcb->nombre_archivo,(datos->nro_bloque),(datos->nro_bloque+ cant_bloques_swap()) - 1);
		 }
	 }

     list_destroy_and_destroy_elements(lista_bloques, &free);

	 usleep(retardo_acceso_fat() * 1000);
	 usleep(retardo_acceso_bloque() * 1000);

}

/* 	===============================================================================================================
					WRITE
	===============================================================================================================*/


void peticion_escritura(char* nombre_archivo,int puntero, int direccion_fisica, int pid ){
   void* valor_a_escribir = pedir_valor_escritura(pid, direccion_fisica);
	 t_fcb* fcb = (t_fcb*) malloc(sizeof(t_fcb));
	 t_block_file* datos ;
	  fcb = buscar_fcb(nombre_archivo);

	    if(fcb == NULL){
	 	   log_info(logger, "No se encontró el fcb del archivo");

	 	   return;
	    }

	    //obtengo lista de entradas del archivo y obtengo el valor en el indice = puntero

	    int nro_bloque= ceil( puntero/tamanio_bloque() ) ;

	    t_list* lista_bloques = cargar_entradas_archivo(fcb);
	    datos = malloc(sizeof(t_block_file));
	    datos = list_get(lista_bloques , nro_bloque );

	   /* if(datos->valor_bloque == FIN_DE_ARCHIVO){
	    	log_info(logger,"El fseek apunta a EOF");
	    	return;

	    }*/
        write_file(valor_a_escribir, datos->nro_bloque );
        log_info(logger, "Acceso Bloque - Archivo:%s - Bloque Archivo: %d - Bloque FS: %d", fcb->nombre_archivo,datos->nro_bloque,(nro_bloque+ cant_bloques_swap()) -1 );



	  usleep(retardo_acceso_bloque() * 1000);
	  list_destroy_and_destroy_elements(lista_bloques, (void*)free);
}

void write_file(void* valor_a_escribir, int nro_bloque ){
	log_info(logger, "Escrbir en el bloque: %d ", nro_bloque);

		escribir_bloque_datos(valor_a_escribir , nro_bloque );
		free(valor_a_escribir);

}

void* pedir_valor_escritura(int pid, int direccion_fisica){

	//void* recibir_valor_escritura = malloc(tamanio_bloque());
	log_info(logger, "Solicitud a memoria del valor en la DF: %d", direccion_fisica);

   	int socket_write = conectar_con_servidor(ip_memoria(), puerto_memoria(), "MEMORIA");

   	int tamanio_valor = tamanio_bloque();

   	t_paquete* paquete = crear_paquete(_WRITE);
   	agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
   	agregar_a_paquete(paquete, &pid, sizeof(int));
   	agregar_a_paquete(paquete, &tamanio_valor, sizeof(int));
    enviar_paquete(paquete, socket_write);

    recibir_operacion(socket_write);
    t_list* init_params = recibir_paquete(socket_write);

    void* valor_recibido;// = malloc(tamanio_bloque());
	valor_recibido = list_get(init_params, 0);

    liberar_conexion(socket_write);
    //free(valor_recibido);
    log_info(logger, "Valor recibido correctamente");

   // list_destroy_and_destroy_elements(init_params, (void*)free);
	return valor_recibido;
}


/* 	===============================================================================================================
					READ
	===============================================================================================================*/
void peticion_lectura(char* nombre_archivo,int puntero, int direccion_fisica, int pid ){
  	 t_fcb* fcb ; //= (t_fcb*) malloc(sizeof(t_fcb));
	 t_block_file* datos ;
	  fcb = buscar_fcb(nombre_archivo);

	    if(fcb == NULL){
	 	   log_info(logger, "No se encontró el fcb del archivo");

	 	   return;
	    }

	    //obtengo lista de entradas del archivo y obtengo el valor en el indice = puntero

	    int nro_bloque= ceil( puntero/tamanio_bloque() ) ;

	    t_list* lista_bloques = cargar_entradas_archivo(fcb);
	    datos = malloc(sizeof(t_block_file));
	    datos = list_get(lista_bloques , nro_bloque );

	    /*if(datos->valor_bloque == FIN_DE_ARCHIVO){
	    	log_info(logger,"El bloque a leer contiene EOF");
	    	return;

	    }*/
	    //obtengo el valor del bloque y lo envío a memoria
	   // void* valor = malloc(tamanio_bloque());
	    void* valor = leer_bloque_datos(datos->nro_bloque);
       log_info(logger, "Acceso Bloque - Archivo:%s - Bloque Archivo: %d - Bloque FS: %d", fcb->nombre_archivo,datos->nro_bloque,(nro_bloque+ cant_bloques_swap()) );

       enviar_memoria_read(pid, direccion_fisica, valor);

      // free(fcb);
	  usleep(retardo_acceso_bloque() * 1000);
	  free(valor);
	  //free(datos);

}

void enviar_memoria_read(int pid, int direccion_fisica, void* valor){
	int socket_read = conectar_con_servidor(ip_memoria(), puerto_memoria(), "MEMORIA");

	int tamanio_valor = tamanio_bloque();

	t_paquete* paquete = crear_paquete(_READ);
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
	agregar_a_paquete(paquete, valor, tamanio_bloque() );
	agregar_a_paquete(paquete, &tamanio_valor, sizeof(int));
	agregar_a_paquete(paquete, &pid, sizeof(int));
	enviar_paquete(paquete, socket_read);

	(char* )recibir_valor(socket_read);

	liberar_conexion(socket_read);
    //free(valor);
}


/* 	===============================================================================================================
					GENERALES
	===============================================================================================================*/


t_list* cargar_entradas_archivo(t_fcb* fcb){ //Recorre todos los bloques del archivo y los carga en la lista
	t_list* lista_bloques_archivo = list_create();
	t_block_file* datos ;//= malloc(sizeof(t_block_file));
	 int bloque_actual = fcb->bloque_inicial;
	 uint32_t valor_grabado= 0;
	 int cant_bloques_anteriores= ceil( (fcb->tamanio)  / tamanio_bloque() )+ 1;

	 //obtengo valor del bloque EOF
	 int i = 0;

	 while(i < cant_bloques_anteriores  && bloque_actual != FIN_DE_ARCHIVO )
	 {
		 memcpy(&valor_grabado,tablaFat + (bloque_actual * sizeof(uint32_t) ) ,sizeof(uint32_t));
		 datos = malloc(sizeof(t_block_file));
		 datos->nro_bloque = bloque_actual;
		 datos->valor_bloque = valor_grabado;
		 list_add(lista_bloques_archivo, datos );
		 bloque_actual = valor_grabado;
		 log_info(logger,"ACCESO FAT - FCB  del archivo %s, tiene entrada: %d , valor %d", fcb->nombre_archivo, datos->nro_bloque,datos->valor_bloque);
		 i++;

	 }


	 /*t_list_iterator* iterador_lst = list_iterator_create(lista_bloques_archivo);
		     	 t_block_file* carga = NULL;
		     	   int indice = iterador_lst->index;

		     	    while(list_iterator_has_next(iterador_lst)){
		     	        carga = list_iterator_next(iterador_lst);
		     	        indice = iterador_lst->index;
		     	       log_info(logger,"que hay en la lista: %d , valor %d, indice %d", carga->nro_bloque,carga->valor_bloque, indice);

		     	    }
		     	    list_iterator_destroy(iterador_lst);

*/
	 log_info(logger,"Hay en total %d bloques de datos asignados", list_size(lista_bloques_archivo));

	return lista_bloques_archivo;

}
