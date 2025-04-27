/*
 * estructuras.c
 *
 *  Created on: Dec 3, 2023
 *      Author: utnso
 */

#include "estructuras.h"


/* 	===============================================================================================================
					UTILES
	===============================================================================================================*/

t_entrada_tabla* buscar_pagina(int pid, int nro_pagina){
	t_proceso_memoria* proceso = buscar_proceso(pid);
	if(proceso == NULL){
			log_error(logger, "EL PROCESO %d NO FUE CREADO EN MEMORIA", pid);
			abort();
	}

	return buscar_pagina_en_tabla(proceso->tabla_de_paginas, nro_pagina);
}


int buscar_marco(int nro_pagina, int pid){
	t_entrada_tabla* pagina = buscar_pagina(pid, nro_pagina);

	if(pagina->bit_presencia == 1)
		return pagina->marco;
	else
		return -1;
}


t_proceso_memoria* buscar_proceso(int pid){

	int cant_procesos = list_size(procesos_en_memoria);
	t_proceso_memoria* proceso;

	for(int i = 0; i < cant_procesos; i++){
		proceso = list_get(procesos_en_memoria, i);
		if(proceso->pid == pid){
			return proceso;
		}
	}

	return NULL;
}


t_entrada_tabla* buscar_pagina_en_tabla(t_list* tabla_de_paginas, int nro_pagina){
	return list_get(tabla_de_paginas, nro_pagina);
}


bool memoria_llena(){
	int cantidad_marcos = list_size(marcos);

	for(int i = 0; i < cantidad_marcos; i++){
		t_marco* marco = list_get(marcos, i);
		if(!marco->ocupado)
			return false;
	}
	return true;
}


t_pagina_presente* comparar_instante_carga(t_pagina_presente* pagina_1, t_pagina_presente* pagina_2){
	t_pagina_presente* posible_pagina_victima;

	if(pagina_1->pagina->instante_carga <= pagina_2->pagina->instante_carga)
		posible_pagina_victima = pagina_1;
	else
		posible_pagina_victima = pagina_2;

	return posible_pagina_victima;
}


t_pagina_presente* comparar_ultimo_acceso(t_pagina_presente* pagina_1, t_pagina_presente* pagina_2){
	t_pagina_presente* posible_pagina_victima;

	if(pagina_1->pagina->ultimo_acceso <= pagina_2->pagina->ultimo_acceso)
		posible_pagina_victima = pagina_1;
	else
		posible_pagina_victima = pagina_2;

	return posible_pagina_victima;
}


int buscar_marco_libre(){
	int cantidad_marcos = list_size(marcos);

	for(int i = 0; i < cantidad_marcos; i++){
		t_marco* marco = list_get(marcos, i);
		if(!marco->ocupado)
			return i;
	}
	return -1;
}


void actualizar_ultimo_acceso(int pid, int direccion_fisica){
	int nro_pagina = buscar_nro_pagina(pid, direccion_fisica);	// MEDIO REDUNDANTE
	t_entrada_tabla* pagina = buscar_pagina(pid, nro_pagina);

	pagina->ultimo_acceso = instante_acceso; instante_acceso++;
}


void actualizar_bit_modificado(int pid, int direccion_fisica){
	int nro_pagina = buscar_nro_pagina(pid, direccion_fisica);
	t_entrada_tabla* pagina = buscar_pagina(pid, nro_pagina);

	pagina->bit_modificado = 1;
}


int buscar_nro_pagina(int pid, int direccion_fisica){
	int nro_marco = direccion_fisica / tam_pagina();
	int nro_pagina = buscar_pagina_por_marco(pid, nro_marco);
	if(nro_pagina >= 0)
		return nro_pagina;
	else{
		log_error(logger, "NO se encontra una pagina para la DIR FISICA: %d", direccion_fisica);
		abort();
	}
}


int buscar_pagina_por_marco(int pid, int nro_marco){
	int cant_paginas = list_size(lista_paginas_presentes);
	t_pagina_presente* pagina;

	for(int i = 0; i < cant_paginas; i++){
		pagina = list_get(lista_paginas_presentes, i);
		if(pagina->pid == pid && pagina->pagina->marco == nro_marco){
			return pagina->nro_pagina;
		}
	}
	return -1;
}


int transformer_pagina_a_bloque_swap(t_entrada_tabla* pagina){
	return pagina->bloque_SWAP;
}


void* buscar_contenido_pagina(int nro_marco){
	void* contenido_pagina = malloc(tam_pagina());
	memcpy(contenido_pagina, memoria_usuario + (nro_marco * tam_pagina()), tam_pagina());
	return contenido_pagina;
}

/* 	===============================================================================================================
					CREACION DE ESTRUCTURAS
	===============================================================================================================*/

void inicializar_estructuras(){
	memoria_usuario = malloc(tam_memoria());
	procesos_en_memoria = list_create();
	marcos = crear_marcos();
	lista_paginas_presentes = list_create();

	instante_carga = 0;
	instante_acceso = 0;
}


t_list* crear_marcos(){
	t_list* marcos = list_create();

	int cantidad_marcos = tam_memoria() / tam_pagina();

	for(int i = 0; i < cantidad_marcos; i++){
		t_marco* marco = malloc(sizeof(t_marco));
		marco->ocupado = 0;
		list_add(marcos, marco);
	}

	return marcos;
}


void crear_proceso(int pid, char* nombre_proceso, int tamanio_proceso){
	t_proceso_memoria* proceso = malloc(sizeof(t_proceso_memoria));

	proceso->pid = pid;
	proceso->nombre = strdup(nombre_proceso);
	proceso->tamanio = tamanio_proceso;
	proceso->tabla_de_paginas = crear_tabla_de_paginas(pid, tamanio_proceso, tam_pagina());
	list_add(procesos_en_memoria, proceso);

	t_list* bloques_SWAP = send_init_proceso_fs(tamanio_proceso);
	actualizar_referencias_a_SWAP(proceso->tabla_de_paginas, bloques_SWAP);

	list_destroy(bloques_SWAP);
}


void actualizar_referencias_a_SWAP(t_list* tabla_de_paginas, t_list* bloques_SWAP){
	int cantidad_entradas = list_size(tabla_de_paginas);

	for(int i = 0; i < cantidad_entradas; i++){
		t_entrada_tabla* pagina = list_get(tabla_de_paginas, i);

		pagina->bloque_SWAP = list_get(bloques_SWAP, i);
	}
}


t_list* crear_tabla_de_paginas(int pid, int tamanio_proceso, int tamanio_pagina){

	t_list* tabla_de_paginas = list_create();

	int cantidad_entradas = ceil(tamanio_proceso / tamanio_pagina);
	// SE SUPONE QUE LOS PARAMETROS VAN A SER SIEMPRE POTENCIAS DE 2, PERO POR LAS DUDAS USO ceil()

	for(int i = 0; i < cantidad_entradas; i++){
		t_entrada_tabla* pagina = crear_pagina();
		list_add(tabla_de_paginas, pagina);
	}
	log_info(logger, "TABLA DE PAGINAS CREADA: PID: %d - Tamaño: %d PAGINAS", pid, cantidad_entradas);

	return tabla_de_paginas;
}


t_entrada_tabla* crear_pagina(){
	t_entrada_tabla* pagina = malloc(sizeof(t_entrada_tabla));
	pagina->bit_modificado = 0;
	pagina->bit_presencia = 0;
	//pagina->ultimo_acceso = instante_acceso; instante_acceso++;
	//pagina->instante_carga = instante_carga; instante_carga++;
	return pagina;
}


t_pagina_presente* crear_pagina_presente(int pid, int nro_pagina, t_entrada_tabla* nueva_pagina){
	t_pagina_presente* nueva_pagina_presente = malloc(sizeof(t_pagina_presente));

	nueva_pagina_presente->pid = pid;
	nueva_pagina_presente->nro_pagina = nro_pagina;
	nueva_pagina_presente->pagina = nueva_pagina;	//TODO: PROBAR

	return nueva_pagina_presente;
}


/* 	===============================================================================================================
					DESTRUCCION DE ESTRUCTURAS
	===============================================================================================================*/


void liberar_estructuras(){
	list_destroy_and_destroy_elements(marcos, (void*)free);
	list_destroy_and_destroy_elements(procesos_en_memoria, (void*)destruir_proceso);
	list_destroy_and_destroy_elements(lista_paginas_presentes, (void*)destruir_pagina_presente);
	free(memoria_usuario);
}


void destruir_pagina_presente(t_pagina_presente* pagina){
	free(pagina);	// NO HAY QUE LIBERAR t_entrada
}


void destruir_paginas_presentes(t_proceso_memoria* proceso){
	int cant_paginas = list_size(lista_paginas_presentes);
	t_pagina_presente* pagina;

	for(int i = 0; i < cant_paginas; i++){
		pagina = list_get(lista_paginas_presentes, i);
		if(pagina->pid == proceso->pid){
			list_remove_element(lista_paginas_presentes, pagina);
			destruir_pagina_presente(pagina);
			i--;
			cant_paginas--;
		}
	}
}


void destruir_proceso(t_proceso_memoria* proceso){
	list_remove_element(procesos_en_memoria, proceso);
	destruir_paginas_presentes(proceso);
	free(proceso->nombre);
	liberar_bloques_swap(proceso->tabla_de_paginas);
	list_destroy_and_destroy_elements(proceso->tabla_de_paginas, (void*)destruir_pagina);
	free(proceso);
}


void destruir_pagina(t_entrada_tabla* pagina){
	if(pagina->bit_presencia == 1){
		t_marco* marco = list_get(marcos, pagina->marco);
		marco->ocupado = 0;
	}
	free(pagina);
}


void liberar_bloques_swap(t_list* tabla_de_paginas){
	t_list* bloques_SWAP = list_map(tabla_de_paginas, (void*)transformer_pagina_a_bloque_swap);
	send_fin_proceso_fs(bloques_SWAP);
	list_destroy(bloques_SWAP);
}

/* 	===============================================================================================================
					PAGE FAULT
	===============================================================================================================*/


void atender_page_fault(int pid, int nro_pagina){
	int marco_libre;
	if(memoria_llena())
		marco_libre = seleccionar_pagina_victima(pid, nro_pagina);
	else
		marco_libre = buscar_marco_libre();

	log_info(logger, "SWAP IN -  PID: %d - Marco: %d - Page In: %d (PID) - %d (PAGINA)", pid, marco_libre, pid, nro_pagina);
	t_entrada_tabla* nueva_pagina = buscar_pagina(pid, nro_pagina);
	void* contenido_pagina = send_swap_in(nueva_pagina->bloque_SWAP);
	memcpy(memoria_usuario + (marco_libre * tam_pagina()), &contenido_pagina, tam_pagina());	// TAM_PAGINA = TAM_BLOQUE
	nueva_pagina->marco = marco_libre;
	nueva_pagina->bit_presencia = 1;
	nueva_pagina->instante_carga = instante_carga; instante_carga++;
	nueva_pagina->ultimo_acceso = instante_acceso; instante_acceso++;	// CUENTA COMO ACCESO??

	t_pagina_presente* nueva_pagina_presente = crear_pagina_presente(pid, nro_pagina, nueva_pagina);
	list_add(lista_paginas_presentes, nueva_pagina_presente);

	t_marco* marco = list_get(marcos, marco_libre);
	marco->ocupado = 1;
	free(contenido_pagina);
}


int seleccionar_pagina_victima(int pid, int nro_pagina){
	t_pagina_presente* pagina_victima;
	t_marco* marco;
	int marco_libre;

	if(strcmp(algoritmo_reemplazo(), "FIFO") == 0)
		pagina_victima = list_get_minimum(lista_paginas_presentes, (void*)comparar_instante_carga);
	else if (strcmp(algoritmo_reemplazo(), "LRU") == 0)
		pagina_victima = list_get_minimum(lista_paginas_presentes, (void*)comparar_ultimo_acceso);
	else
		log_error(logger, "Algoritmo de reemplazo desconocido");

	pagina_victima->pagina->bit_presencia = 0;
	list_remove_element(lista_paginas_presentes, pagina_victima);
	marco_libre = pagina_victima->pagina->marco;
	log_info(logger, "REEMPLAZO - Marco: %d - Page Out: %d (PID) - %d (PAGINA) - Page In: %d (PID) - %d (PAGINA)", marco_libre, pagina_victima->pid, pagina_victima->nro_pagina, pid, nro_pagina);

	if(pagina_victima->pagina->bit_modificado == 1){
		log_info(logger, "SWAP OUT -  PID: %d - Marco: %d - Page Out: %d (PID) - %d (PAGINA)", pid, pagina_victima->pagina->marco, pagina_victima->pid, pagina_victima->nro_pagina);
		void* contenido_pagina = buscar_contenido_pagina(pagina_victima->pagina->marco);
		send_swap_out(contenido_pagina, pagina_victima->pagina->bloque_SWAP);
		free(contenido_pagina);
	}

	destruir_pagina_presente(pagina_victima);

	marco = list_get(marcos, marco_libre);
	marco->ocupado = 0;

	return marco_libre;
}


/* 	===============================================================================================================
					PETICIONES A FILESYSTEM
	===============================================================================================================*/

t_list* send_init_proceso_fs(int tamanio_proceso){
	int socket_init_fs = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
	t_paquete* paquete = crear_paquete(_INICIAR_PROCESO);
	agregar_a_paquete(paquete, &tamanio_proceso, sizeof(int));
	enviar_paquete(paquete, socket_init_fs);

	t_list* bloques_SWAP = list_create();
	recibir_operacion(socket_init_fs);
	t_list* init_params = recibir_paquete(socket_init_fs);

	int* cant_bloques = list_get(init_params, 0);
	for (int i = 1; i <= *cant_bloques; i++){
		int *nro_bloque_swap = list_get(init_params, i);  //log_info(logger,"valor i %d, bloque %d", i, *nro_bloque_swap );
		list_add(bloques_SWAP, *nro_bloque_swap);
	}

	liberar_conexion(socket_init_fs);
	list_destroy_and_destroy_elements(init_params, (void*)free);
	return bloques_SWAP;
}




void send_fin_proceso_fs(t_list* bloques_SWAP){
	int socket_fin_fs = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
	t_paquete* paquete = crear_paquete(_FINALIZAR_PROCESO);

	int cantidad_bloques = list_size(bloques_SWAP);
    agregar_a_paquete(paquete, &cantidad_bloques, sizeof(int));
	 for (int i = 0; i < cantidad_bloques; i++){
		 int nro = list_get(bloques_SWAP, i);
	     agregar_a_paquete(paquete, &(nro), sizeof(int)); //log_info(logger,"serializo valor %d", nro);
	 }
	 enviar_paquete(paquete, socket_fin_fs);

}


void* send_swap_in(int nro_bloque){
	int socket_swap_in = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
	t_paquete* paquete = crear_paquete(_SWAP_IN);
	agregar_a_paquete(paquete, &nro_bloque, sizeof(int));
	enviar_paquete(paquete, socket_swap_in);

	recibir_operacion(socket_swap_in);
	t_list* init_params = recibir_paquete(socket_swap_in);
	void* contenido_pagina; // = malloc(tam_pagina());
	contenido_pagina = list_get(init_params, 0);

	liberar_conexion(socket_swap_in);
	list_destroy(init_params);
	//list_destroy_and_destroy_elements(init_params, (void*)free);
	return contenido_pagina;
}


void send_swap_out(void* contenido_pagina, int nro_bloque){
	int tamanio_pagina = strlen(contenido_pagina) + 1; // ¿tam_pagina() del config?

	int socket_swap_out = conectar_con_servidor(ip_filesystem(), puerto_filesystem(), "FILESYSTEM");
	t_paquete* paquete = crear_paquete(_SWAP_OUT); // OP_CODE: SWAP_OUT
	agregar_a_paquete(paquete, contenido_pagina, tam_pagina());
	agregar_a_paquete(paquete, &nro_bloque, sizeof(int));
	enviar_paquete(paquete, socket_swap_out);

	// RECIBIR OK?
	liberar_conexion(socket_swap_out);
}


int conectar_con_servidor(char* ip, char* puerto, char* servidor){
	log_info(logger, "Conectando con servidor %s en IP: %s y Puerto: %s", servidor, ip, puerto);

	int socket = crear_conexion(ip, puerto);
	if (socket < 0){
		log_error(logger,"No se pudo conectar con el servidor %s", servidor);
		return EXIT_FAILURE;
	}
	log_info(logger, "El socket conectado es: %d", socket);

	int result = handshakeCliente(socket, MEMORIA);
	if(result == 0)
		log_info(logger, "Handshake con %s exitoso", servidor);
	else{
		log_error(logger, "Handshake con %s fallido", servidor);
		return EXIT_FAILURE;
	}

	//enviar_mensaje("HOLA, SOY KERNEL \n", socket); //PARA PROBAR

	return socket;	// COMO YA MANEJO LOS ERRORES SIEMPRE RETORNA CORRECTAMENTE EL FILE DESCRIPTOR, NO RETORNA -1
}
