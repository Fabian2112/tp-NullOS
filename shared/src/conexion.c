/*
 * conexion.c
 *
 *  Created on: Sep 9, 2023
 *      Author: utnso
 */
#include "conexion.h"

struct addrinfo *addrinfo_servidor(char *ip, char *puerto) {
	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	return server_info;
}

int crear_socket(struct addrinfo *server_info) {
	return socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
}

// servidor

int iniciar_servidor(char *puerto) {
	struct addrinfo *server_info = addrinfo_servidor(NULL, puerto);
	int socket_servidor = crear_socket(server_info);
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));  //CAMBIAR SO_REUSEADDR POR SO_REUSEPORT
	bind(socket_servidor, server_info->ai_addr, server_info->ai_addrlen);
	//printf("Servidor iniciado en puerto: %s\n", puerto);
	listen(socket_servidor, SOMAXCONN);
	freeaddrinfo(server_info);
	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
    struct sockaddr_in dir_cliente;

    int tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void *)&dir_cliente, &tam_direccion);

   return socket_cliente;
}

int recibir_operacion(int socket_cliente) {
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else {
		close(socket_cliente);
		return -1;
	}
}

void *recibir_buffer(int *size, int socket_cliente) {
	void *buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	assert(buffer != NULL);

	recv(socket_cliente, buffer, *size, MSG_WAITALL); //printf("Recibi %d bytes", size[0]);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
    int size;
    char *buffer = recibir_buffer(&size, socket_cliente);
    printf("Me llego el mensaje %s", buffer);
    free(buffer);
}

t_list *recibir_paquete(int socket_cliente)
{
    int size;
    int desplazamiento = 0;
    void *buffer;
    t_list *valores = list_create();
    int tamanio;

    buffer = recibir_buffer(&size, socket_cliente);
    while (desplazamiento < size)
    {
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));		// SE COPIA EL TAMANIO DEL VALOR
        desplazamiento += sizeof(int);
        char *valor = malloc(tamanio);	// SE RESERVA MEMORIA PARA EL VALOR
        memcpy(valor, buffer + desplazamiento, tamanio);	// SE COPIA EL VALOR
        desplazamiento += tamanio;
        list_add(valores, valor);	// SE GUARDA EL VALOR EN UNA LISTA
    }
    free(buffer);
    return valores;
}


// cliente

int crear_conexion(char *ip, char *puerto) {
	struct addrinfo *server_info = addrinfo_servidor(ip, puerto);
	int socket_cliente = crear_socket(server_info);

	setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));  //CAMBIAR SO_REUSEADDR POR SO_REUSEPORT

	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		return -1;

	freeaddrinfo(server_info);
	return socket_cliente;
}

t_buffer *crear_buffer() {
	t_buffer *buffer = malloc(sizeof(t_buffer));
	assert(buffer != NULL);

	buffer->size = 0;
	buffer->stream = NULL;
	return buffer;
}

t_paquete *crear_paquete(int codigo_operacion) {
	t_paquete *paquete = malloc(sizeof(t_paquete));
	assert(paquete != NULL);

	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = crear_buffer();
	return paquete;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int bytes) {
	//t_buffer *buffer = paquete->buffer;
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + bytes + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &bytes, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, bytes);

	paquete->buffer->size += bytes + sizeof(int);
}

void agregar_a_buffer(t_buffer* buffer, void* valor, int bytes) {
	buffer->stream = realloc(buffer->stream, buffer->size + bytes);
	memcpy(buffer->stream + buffer->size, valor, bytes);
	buffer->size += bytes;
}


void *serializar_paquete(t_paquete *paquete, int size_serializado) {
	void *magic = malloc(size_serializado);
	assert(magic != NULL);

	int desplazamiento = 0;
	t_buffer *buffer = paquete->buffer;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(magic + desplazamiento, &(buffer->size), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(magic + desplazamiento, buffer->stream, buffer->size);

	return magic;
}

void eliminar_paquete(t_paquete *paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_buffer(t_buffer* buff){
	free(buff->stream);
	free(buff);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente) {
	int size_serializado = paquete->buffer->size + 2 * sizeof(int);
	void *a_enviar = serializar_paquete(paquete, size_serializado);
	assert(send(socket_cliente, a_enviar, size_serializado, 0) > 0);
	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_mensaje(char *mensaje, int socket_cliente)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = MENSAJE;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

    int bytes = paquete->buffer->size + 2 * sizeof(int);

    void *a_enviar = serializar_paquete(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    eliminar_paquete(paquete);
}

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}

void* sacar_de_buffer(t_buffer* buffer, int tam_dato) {

	assert(buffer!=NULL);

	void* buffer_nuevo = malloc(buffer->size-tam_dato);
	assert(buffer_nuevo != NULL);

	void* dato = malloc(tam_dato);
	assert(dato != NULL);

	memcpy(dato, buffer->stream, tam_dato);
	memcpy(buffer_nuevo, buffer->stream+tam_dato, (buffer->size-tam_dato));

	free(buffer->stream);

	buffer->size -= tam_dato;
	buffer -> stream = buffer_nuevo;

	return dato;
}

//SERIALIZACION Y DESERIALIZACION DE MENSAJES ENTRE MODULOS

void serializar_pcb(t_paquete *paquete, t_pcb *pcb){

	//agregar_a_paquete(paquete,&nombre_archivo,strlen(nombre_archivo)+1); //char
	//agregar_a_paquete(paquete,&id_proceso,sizeof(int));


}

void serializar_contexto(t_paquete *paquete, t_contexto* contexto){
	//t_buffer* buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size =  2 * sizeof(int) + sizeof(t_registros) ; //pid+ pc +registros
	paquete->buffer->stream = malloc(paquete->buffer->size);

	int desplazamiento= 0;

	   memcpy(paquete->buffer->stream + desplazamiento, &contexto->pid, sizeof(int));
	   desplazamiento+= sizeof(int);
	   memcpy(paquete->buffer->stream + desplazamiento, &contexto->program_counter, sizeof(int));
	   desplazamiento+= sizeof(int);
	   memcpy(paquete->buffer->stream + desplazamiento, &(contexto->registros->AX), sizeof(uint32_t));
	   desplazamiento+= sizeof(uint32_t);
	   memcpy(paquete->buffer->stream + desplazamiento, &(contexto->registros->BX), sizeof(uint32_t));
	   desplazamiento+= sizeof(uint32_t);
	   memcpy(paquete->buffer->stream + desplazamiento, &(contexto->registros->CX), sizeof(uint32_t));
	   desplazamiento+= sizeof(uint32_t);
	   memcpy(paquete->buffer->stream + desplazamiento, &(contexto->registros->DX), sizeof(uint32_t));
	   desplazamiento+= sizeof(uint32_t);
		//paquete->buffer = buffer;


	/*agregar_a_paquete(paquete,&contexto->pid, sizeof(int));
	agregar_a_paquete(paquete,&contexto->program_counter, sizeof(int));
	agregar_a_paquete(paquete,&contexto->registros,  sizeof(t_registros) );
	agregar_a_paquete(paquete,&contexto->registros->AX, sizeof(uint32_t));
	agregar_a_paquete(paquete,&contexto->registros->BX, sizeof(uint32_t));
	agregar_a_paquete(paquete,&contexto->registros->CX, sizeof(uint32_t));
	agregar_a_paquete(paquete,&contexto->registros->DX, sizeof(uint32_t));
*/

}

t_contexto* deserializar_contexto(int socket){
	t_contexto* contexto = (t_contexto*) malloc(sizeof(t_contexto));
		contexto->registros = malloc(sizeof(t_registros));
		int desplazamiento = 0;

	     t_buffer* buffer = crear_buffer();
	     	 buffer-> stream = recibir_buffer(&(buffer -> size), socket); //printf("Se recibió %d bytes", buffer->size);
	    assert(contexto != NULL);
		    memcpy(&(contexto->pid), buffer->stream + desplazamiento , sizeof(int));
			desplazamiento += sizeof(int);
			memcpy(&(contexto->program_counter), buffer->stream + desplazamiento , sizeof(int));
			desplazamiento += sizeof(int);
			memcpy(&(contexto->registros->AX), buffer->stream + desplazamiento , sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			memcpy(&(contexto->registros->BX), buffer->stream + desplazamiento , sizeof(uint32_t));
		    desplazamiento += sizeof(uint32_t);
		    memcpy(&(contexto->registros->CX), buffer->stream + desplazamiento , sizeof(uint32_t));
		    desplazamiento += sizeof(uint32_t);
		    memcpy(&(contexto->registros->DX), buffer->stream + desplazamiento , sizeof(uint32_t));
		    desplazamiento += sizeof(uint32_t);

	liberar_buffer(buffer);
	return contexto;
}

void enviarContexto(t_contexto* contexto_ejecucion,int op_code, int socket){
	t_paquete* paquete = crear_paquete(op_code);
    serializar_contexto(paquete, contexto_ejecucion);
	enviar_paquete(paquete, socket);

}

void serializar_peticion_instruccion(t_paquete* paquete, t_peticion_instruccion* pedir_instruccion){  //Pedir a memoria sgte instrucción
	agregar_a_paquete(paquete,&pedir_instruccion->pid, sizeof(int));
	agregar_a_paquete(paquete,&pedir_instruccion->program_counter, sizeof(int));
}


t_peticion_instruccion* deserializar_peticion_instruccion(int socket) //En memoria recibir esta estructura
{
   t_peticion_instruccion* peticion_instruccion = (t_peticion_instruccion*) malloc(sizeof(t_peticion_instruccion));
  int desplazamiento = 0;

   t_buffer* buffer = crear_buffer();
   	   buffer-> stream = recibir_buffer(&(buffer -> size), socket);
   assert(peticion_instruccion != NULL);
		    memcpy(&(peticion_instruccion->pid), buffer->stream + desplazamiento, sizeof(int));
			desplazamiento += sizeof(int);  //printf("Desplazamiento post pid %d", desplazamiento);
			memcpy(&(peticion_instruccion->program_counter), buffer->stream + desplazamiento, sizeof(int));
			desplazamiento += sizeof(int);  // printf("Desplazamiento post pid %d", desplazamiento);
	liberar_buffer(buffer);
	return peticion_instruccion;
}

void serializar_proceso_nuevo(t_paquete* paquete, t_memoria_instruccion* proceso_nuevo) //Enviar a memoria datos para estructura de instrucciones
{
	proceso_nuevo->nombre_length =strlen(proceso_nuevo->nombre_archivo) + 1 ; // printf("tamaño del archivo %d ", proceso_nuevo->nombre_length);

	//t_buffer* buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = proceso_nuevo->nombre_length + 2 * sizeof(int)  ; //char + int pid + int del largo
	paquete->buffer->stream = malloc(paquete->buffer->size);

	int desplazamiento= 0;

	   memcpy(paquete->buffer->stream + desplazamiento, &proceso_nuevo->pid, sizeof(int));
	   desplazamiento+= sizeof(int);
	   memcpy(paquete->buffer->stream + desplazamiento, &(proceso_nuevo->nombre_length), sizeof(int));  //tamaño del char
	   desplazamiento+= sizeof(int);
		memcpy(paquete->buffer->stream + desplazamiento, proceso_nuevo->nombre_archivo, proceso_nuevo->nombre_length);  //char

		//paquete->buffer = buffer;

	  // printf("Envio %d en total" , paquete->buffer->size);
}


t_memoria_instruccion* deserializar_proceso_nuevo(int socket) //En memoria recibir para crear estructura para pet. de instrucciones
{
    t_memoria_instruccion* proceso_nuevo = (t_memoria_instruccion*) malloc(sizeof(t_memoria_instruccion));
    int desplazamiento = 0;

     t_buffer* buffer = crear_buffer();
     	 buffer-> stream = recibir_buffer(&(buffer -> size), socket); //printf("Se recibió %d bytes", buffer->size);
     assert(proceso_nuevo != NULL);
        memcpy(&(proceso_nuevo->pid), buffer->stream + desplazamiento , sizeof(int));
     	desplazamiento+= sizeof(int);
     	memcpy(&(proceso_nuevo->nombre_length),buffer->stream + desplazamiento, sizeof(int));
     	desplazamiento+= sizeof(int);
     	proceso_nuevo->nombre_archivo= malloc(proceso_nuevo->nombre_length);
     	memcpy(proceso_nuevo->nombre_archivo, buffer->stream + desplazamiento, proceso_nuevo->nombre_length);

     liberar_buffer(buffer);
	return proceso_nuevo;
}

char* recibir_valor(int socket_cliente)
{
    char* mensaje;
    int size;
	         switch (recibir_operacion(socket_cliente))
	         {
	         case MENSAJE:
	          mensaje= recibir_buffer(&size, socket_cliente);  // printf("Recibí en el mensaje %d bytes", size);
              break;

	         default:
	           break;
	         }


    return mensaje;
}
