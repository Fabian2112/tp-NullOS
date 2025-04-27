#include "estructuras.h"

bool inicializar_estructuras(){
	entrada_fat = (uint32_t)(cant_bloques_total() - cant_bloques_swap() ); log_info(logger,"entrada_fat %d", entrada_fat);

   //Cargar lista de fcbs
	if ( !cargar_fcbs() )  return false;
	if( !inicializar_tabla_fat() ) return false; // cargo todas las entradas en 0


	if(! inicializar_tabla_datos() ) return false;

int file_fat = open(path_fat(), O_RDWR, S_IRUSR | S_IWUSR);
tablaFat = mmap(NULL, sizeof(uint32_t), PROT_READ | PROT_WRITE, MAP_SHARED, file_fat, 0);



    struct stat fsblqs;

    if(fstat(file_dat, &fsblqs) == -1) {
        log_error(logger, "No se pudo abrir el archivo de bloques");
        log_destroy(logger);
        return EXIT_FAILURE;
    }

    int file_dat = open(path_bloques(), O_RDWR, S_IRUSR | S_IWUSR);
    bloques_datos = mmap(NULL, cant_bloques_total() * tamanio_bloque(), PROT_READ | PROT_WRITE, MAP_SHARED , file_dat, 0);


	cargar_lista_swap();

	//CLOSE en fyleSystem.c


return true;

}

/* 	===============================================================================================================
					BLOQUE DE SWAP
	===============================================================================================================*/
void escribir_bloque_swap(void* contenido_a_escribir, uint32_t nro_bloque ){
	//Escribo
	memcpy(bloques_datos + nro_bloque * tamanio_bloque() , contenido_a_escribir , tamanio_bloque() );

}

void* leer_bloque_swap(uint32_t nro_bloque ){
	void*contenido_a_leer= NULL;
	contenido_a_leer =malloc(tamanio_bloque());

	//Leo
   memcpy(contenido_a_leer,bloques_datos + nro_bloque * tamanio_bloque() , tamanio_bloque());

   return contenido_a_leer;

}


int siguiente_bloque_swap_libre(){

	t_swap_block* buscar_sgte; //= malloc(sizeof(t_swap_block));

	for(int i = 0; i <  (cant_bloques_swap() - 1 ) ; i++){
	 buscar_sgte = malloc(sizeof(t_swap_block));

	 buscar_sgte= list_get(lista_swaps , i);
	 if (buscar_sgte->ocupado != 0){
		 continue;
	 }
	 return i;

	  }
   return -1;

}

void liberar_bloque_swap(int nro_bloque){
	//Grabar en el bloque el valor fijo
	void*  valor_en_bloque = malloc(tamanio_bloque());
	 // uint32_t valor = 0;

	memset(valor_en_bloque, 0, tamanio_bloque());
	memcpy(bloques_datos + nro_bloque * tamanio_bloque() , valor_en_bloque , tamanio_bloque() );

	//Agregar a la lista valor= 1
	t_swap_block* actualizar = malloc(sizeof(t_swap_block));
	 actualizar->nro_bloque= nro_bloque;
	 actualizar->ocupado = 0;

		list_replace(lista_swaps ,nro_bloque,actualizar); log_info(logger,"ACCESO A SWAP, se libera bloque: %d", nro_bloque);
free(valor_en_bloque);
}

void inicializar_bloque_swap(int nro_bloque){
	//Grabar en el bloque el valor fijo
	void* contenido_a_escribir = malloc(tamanio_bloque());

	memset(contenido_a_escribir, '\0', tamanio_bloque());

	memcpy(bloques_datos + nro_bloque * tamanio_bloque() , contenido_a_escribir , tamanio_bloque() );

	//Agregar a la lista valor= 1
	 t_swap_block* actualizar = malloc(sizeof(t_swap_block));
	 actualizar->nro_bloque= nro_bloque;
	 actualizar->ocupado = 1;

	list_replace(lista_swaps ,nro_bloque,actualizar);
free(contenido_a_escribir);

}


void cargar_lista_swap(){
lista_swaps = list_create();

	  	    for(int i = 0; i <  (cant_bloques_swap() ) ; i++){
	  	    t_swap_block*  init = malloc(sizeof(t_swap_block));
	  	    init->nro_bloque = i;
	  	    init->ocupado=0;
	  	    list_add(lista_swaps, init);
    	    }

	  	     /* t_list_iterator* iterador_lst = list_iterator_create(lista_swaps);
	  	       t_swap_block* carga =NULL;

  			     	    while(list_iterator_has_next(iterador_lst)){
    	     	        carga = list_iterator_next(iterador_lst);

	  			     	    log_info(logger,"Bloque  %d, ocupado? %d", carga->nro_bloque, carga->ocupado);

	  			     	    }
	  			 list_iterator_destroy(iterador_lst);
*/



	  log_info(logger,"Lista Adm. para administrar %d bloques swap", list_size(lista_swaps));
}



/* 	===============================================================================================================
					BLOQUE DE DATOS FAT
	===============================================================================================================*/
void escribir_bloque_datos(void* contenido_a_escribir, uint32_t nro_bloque ){
	uint32_t nro_bloque_datos = nro_bloque + cant_bloques_swap()- 1;

	//Escribo
	memcpy(bloques_datos + (nro_bloque_datos * tamanio_bloque()) , contenido_a_escribir , tamanio_bloque() ); log_info(logger, "Escritura en el bloque de datos: %d", nro_bloque_datos);


}

void* leer_bloque_datos(uint32_t nro_bloque ){
	void* contenido_a_leer= NULL;
		contenido_a_leer =malloc(tamanio_bloque());

   uint32_t nro_bloque_datos = nro_bloque + cant_bloques_swap() - 1; log_info(logger, "llego del bloque: %d", nro_bloque);


    //Leo
   memcpy(contenido_a_leer,bloques_datos + nro_bloque_datos * tamanio_bloque() , tamanio_bloque()); log_info(logger, "Lectura del bloque de datos: %d", nro_bloque_datos);



    //	log_info(logger, "Leí el valor: %d , bloque:  %d en archivo de datos", *(uint32_t*)contenido_a_leer, nro_bloque_datos);

    	return contenido_a_leer;

}

bool inicializar_tabla_datos(){


	FILE* file_datos = fopen(path_bloques(), "wb");  // "wb"
						    if (file_datos == NULL) {
						       log_info(logger, "Error en abrir el archivo de bloques");
						        return false;
						    }

						   int fd = fileno(file_datos);
						      if (fd == -1) {
						          perror("Error al obtener el descriptor de archivo");
						          fclose(file_datos);
						      }

						    if (ftruncate(fd , (cant_bloques_total() * tamanio_bloque() ) ) == -1) {
						           perror("Error al truncar el archivo");
						           fclose(file_datos);
						           exit(EXIT_FAILURE);
						       }
						  fclose(file_datos);


/*

	 // Abrir el archivo
	   FILE* bloques_datos = fopen(path_bloques(), "wb");  // "wb"
	    if (bloques_datos == NULL) {
	       log_info(logger, "Error en abrir el archivo de bloques");
	        return false;
	    }


   //inicializo en 0
	  char* buffer = malloc(tamanio_bloque() * cant_bloques_total() );
	  memset(buffer, 0 , tamanio_bloque());
	  uint32_t i;
	    for (i = 0; i < cant_bloques_total() ; i++) {
	    		fwrite(buffer,tamanio_bloque(),1,bloques_datos);

	    		}

    // Cerrar el archivo después de escribir
	    fclose(bloques_datos);

	    log_info(logger,"Se inicializaron %d bloques en bloques.dat. Los primeros %d son de swap\n", i, cant_bloques_swap());

	    free(buffer);*/
	    return true;

}


/* 	===============================================================================================================
					TABLA FAT
	===============================================================================================================*/

uint32_t buscar_primera_entrada_libre(){
	uint32_t valor = 0;
	  	    for(uint32_t i = 1; i < entrada_fat ; i++){

	  	    	memcpy(&valor , tablaFat +  i * sizeof(uint32_t), sizeof(uint32_t));

    	       if (valor == 0) {
 	    	   		// log_info(logger, "Entrada FAT  %d", i);
    	    	   return i;
    	       }
    	    }
   return -1;

}

int cantidad_entradas_libres(){
	  int cantidad = 0;
	  uint32_t valor = 0;
	  	    for(int i = 0; i < entrada_fat ; i++){

	    	       if ( tablaFat[i] == valor) {
	 	    	   		 cantidad++;
	    	       }

	  	    }
   return cantidad - 1 ;  //BLOQUE CERO NO SE ASIGNA
}


void agregar_entrada(uint32_t valor, uint32_t nro_bloque ){
	uint32_t valor_grabado = 0;


	memcpy(tablaFat + nro_bloque * sizeof(uint32_t), &valor ,sizeof(uint32_t));


    memcpy(&valor_grabado,tablaFat + nro_bloque * sizeof(uint32_t) ,sizeof(uint32_t));

    if (valor != -1){  //Distinto de EOF

    	//log_info(logger, "Se grabó el valor : %d , en la entrada fat %d ", valor, nro_bloque);
    	//log_info(logger, "Se grabó el valor : %d , en la entrada fat %d ", (uint32_t)valor_grabado, nro_bloque);

    }

   // log_info(logger, "Acceso FAT - Entrada: %d - Valor: %d", nro_bloque, (uint32_t)valor_grabado);


}

bool inicializar_tabla_fat(){
	// Abrir el archivo
	   FILE* tabla_fat = fopen(path_fat(), "wb");//wb
	    if (tabla_fat == NULL) {
	       log_info(logger, "Error en abrir el archivo de la tabla fat");
	        return false;
	    }


	//inicializo en 0
	    uint32_t valor_inicial = 0;
	    uint32_t i;
	    for (i = 0; i < entrada_fat ; i++) {
	    		fwrite(&valor_inicial,sizeof(uint32_t),1,tabla_fat);
	    			}
    // Cerrar el archivo después de escribir
	    fclose(tabla_fat);

	   log_info(logger,"Se inicializaron %d entradas en la fat.\n", i);

	    return true;

}


/* 	===============================================================================================================
					FCB
	===============================================================================================================*/
bool cargar_fcbs(){

	DIR *directorio;
	 struct dirent *entrada;
	//struct
	//*entrada;

//char* path = path_fcb();
t_fcb* fcb;

 //Abrir directorio de fcbs
 directorio = opendir(path_fcb());

   if (directorio == NULL){
	  log_info(logger,"No se pudo abrir el directorio de FCBS");
	return false;
   }

   //CARGO FILE
   lista_fcbs= list_create();
   t_config* fcb_file;

   //Recorrer las entradas del directorio
   while( (entrada= readdir(directorio))  != NULL  ){
	  if( string_equals_ignore_case(entrada->d_name, ".")  || string_equals_ignore_case(entrada->d_name,".." )  ){
		  continue;
	  }

    //Obtener path
	  int  tamanio_ruta = strlen(path_fcb()) + strlen(entrada->d_name) + 2;
	  char* ruta_archivo = malloc(tamanio_ruta);
	  snprintf(ruta_archivo, tamanio_ruta, "%s/%s", path_fcb(), entrada->d_name);

	  //Leer fcb
	  fcb_file = config_create(ruta_archivo);
	  if (fcb_file == NULL) {
	        log_info(logger, "Error al abrir el archivo: %s", ruta_archivo);
	              continue;
	          }
	  fcb = (t_fcb*) malloc (sizeof(t_fcb));

	  fcb->nombre_archivo = config_get_string_value(fcb_file,"NOMBRE_ARCHIVO");
	  fcb->tamanio= 0; //config_get_int_value(fcb_config,"TAMANIO_ARCHIVO");
	  fcb->bloque_inicial = 0;// config_get_int_value(fcb_config,"TAMANIO_ARCHIVO");


	  free(ruta_archivo);
	   list_add(lista_fcbs,fcb);

   }

   int cantidad = list_size(lista_fcbs);
   log_info(logger, "Se cargaron %d fcbs", cantidad);
   //config_destroy(fcb_file);

 closedir(directorio);
 //config_destroy(fcb_file);

 return true;

}


void crear_fcb(char* nombre_archivo){

  t_fcb	*fcb_nuevo = (t_fcb*) malloc (sizeof(t_fcb));
  char *linea = string_new();
  char *path = string_new();
			     string_append(&path , path_fcb() );
			     string_append(&path , "/" );
			     string_append(&path , nombre_archivo );
			    log_info(logger, "El path del archivo es: %s." , path);


    FILE* archivo;
    archivo = fopen(path,"w+");

    fcb_nuevo->nombre_archivo = nombre_archivo;
    fcb_nuevo->tamanio = 0;
    fcb_nuevo->bloque_inicial = 0;

    string_append_with_format(&linea, "%s=%s\n", "NOMBRE_ARCHIVO", nombre_archivo);
    string_append_with_format(&linea, "%s=%s\n", "TAMANIO_ARCHIVO", "0");
    string_append_with_format(&linea, "%s=%s\n", "BLOQUE_INICIAL", "0");

    fwrite(linea, strlen(linea), 1, archivo);

        fclose(archivo);
        list_add(lista_fcbs, fcb_nuevo);
        free(path);
        free(linea);
        //liberarfcb(fcb_nuevo);


        int cantidad = list_size(lista_fcbs);
        int tamanio = strlen(nombre_archivo);
         log_info(logger, "Hay %d fcbs , %d", cantidad, tamanio);

}

t_fcb*  buscar_fcb(char* nombre_archivo){
    t_list_iterator* fcbs = list_iterator_create(lista_fcbs);
    t_fcb* fcb_encontrado = NULL;
    int indice = fcbs->index;

    while(list_iterator_has_next(fcbs)){
        fcb_encontrado = list_iterator_next(fcbs);
        log_info(logger, "NOMBRE FCB: %s", fcb_encontrado->nombre_archivo);
        if(strcmp(fcb_encontrado->nombre_archivo, nombre_archivo) == 0){  //log_info(logger,"File_list %s y file %s",fcb_encontrado->nombre_archivo,nombre_archivo );
        	indice = fcbs->index;
            list_iterator_destroy(fcbs);
            return fcb_encontrado;
        }

    }
    list_iterator_destroy(fcbs);

    if(indice != -1 && strcmp(fcb_encontrado->nombre_archivo, nombre_archivo) == 0)
        return fcb_encontrado;
    return NULL;
}




void modificar_fcb(t_fcb *modificado){
	//modificar en lista

	bool es_fcb(void *_fcb)
			  {
				t_fcb* fcb  = (t_fcb *)_fcb;
			    return fcb->nombre_archivo == modificado->nombre_archivo;
			  }
	list_replace_by_condition(lista_fcbs, &es_fcb, modificado);

	//modificar en archivo

	char *path = string_new();
				     string_append(&path , path_fcb() );
				     string_append(&path , "/" );
				     string_append(&path , modificado->nombre_archivo );

	t_config* fcb_config = config_create(path);
		  if (fcb_config == NULL) {
		        log_info(logger, "Error al abrir el archivo: %s", path);
		             // break;
		          }

	char* tamanio_archivo = string_new();
	char* bloque_inicial = string_new();
	    string_append_with_format(&tamanio_archivo, "%d", modificado->tamanio);
	    string_append_with_format(&bloque_inicial, "%d", modificado->bloque_inicial);

	    config_set_value(fcb_config, "TAMANIO_ARCHIVO", tamanio_archivo);
	    config_set_value(fcb_config, "BLOQUE_INICIAL", bloque_inicial);
	    config_save(fcb_config);

	    free(tamanio_archivo);
	    free(bloque_inicial);
	    free(path);
	   config_destroy(fcb_config);



}


void liberarfcb(t_fcb* fcb) {
	free((void*)fcb->nombre_archivo);
	free(fcb);
}
