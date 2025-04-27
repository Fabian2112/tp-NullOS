/*
 * conexion.h
 *
 *  Created on: Sep 9, 2023
 *      Author: utnso
 */

#ifndef CONEXION_H_
#define CONEXION_H_

#include "TypeDef.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "proceso.h"

typedef struct {
	int size;
	void *stream;
} t_buffer;

typedef struct {
	int codigo_operacion;
	t_buffer *buffer;
} t_paquete;

typedef enum
{
	ERROR = -1,
	MENSAJE,
    PAQUETE,
	_MEMORIA_INSTRUCCIONES,  //kernel a memoria para cargar estructura
	_CONTEXTO,
	_PEDIR_INSTRUCCION,
	_EXIT,   //CPU-KERNEL POR EXEC INSTRUCCIONES
	_SLEEP,  //CPU-KERNEL POR EXEC INSTRUCCIONES
	_WAIT,   //CPU-KERNEL POR EXEC INSTRUCCIONES
	_SIGNAL, //CPU-KERNEL POR EXEC INSTRUCCIONES
	_OPEN,   //CPU-KERNEL POR EXEC INSTRUCCIONES
	_CLOSE,  //CPU-KERNEL POR EXEC INSTRUCCIONES
	_SEEK,   //CPU-KERNEL POR EXEC INSTRUCCIONES
	_READ,   //CPU-KERNEL POR EXEC INSTRUCCIONES
	_WRITE,  //CPU-KERNEL POR EXEC INSTRUCCIONES
	_TRUNCATE, //CPU-KERNEL POR EXEC INSTRUCCIONES
	_PAGE_FAULT,	 //CPU-KERNEL POR EXEC INSTRUCCIONES
	_DESALOJO, //INTERRUPCIONES KERNEL
	_NRO_MARCO,  //PEDIDO DE CPU A MEMORIA
	_MOV_IN, //OBTENER VALOR CPU-MEMORIA
	_MOV_OUT, //GRABAR UN VALOR EN MEMORIA
	_TAMANIO_PAGINA, //OBTENER VALOR PARA TRADUCCION CPU-MEMORIA
	_FINALIZAR_PROCESO, //USADO PARA ELIMINAR ESTRUCTURAS MEMORIA, FILESYSTEM, DESALOJO EN CPU
	_INICIAR_PROCESO, // USADO PARA GENERAR ESTRUCTURAS DE MEMORIA DE USUARIOS
	_SWAP_IN,
	_SWAP_OUT,
} op_code;

struct addrinfo *addrinfo_servidor(char *ip, char *puerto);
int crear_socket(struct addrinfo *server_info);


// servidor

int iniciar_servidor(char *puerto);
int esperar_cliente(int);
void *recibir_buffer(int *, int);
int recibir_operacion(int);
void recibir_mensaje(int socket_cliente);
t_list *recibir_paquete(int);

// cliente

int crear_conexion(char *ip, char *puerto);
t_buffer *crear_buffer();
void liberar_buffer(t_buffer*);
t_paquete *crear_paquete(int codigo_operacion);
void agregar_a_paquete(t_paquete *paquete, void *valor, int bytes);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void enviar_mensaje2(void *mensaje, int bytes, int socket_cliente);
void enviar_mensaje(char* mensaje, int socket_cliente);
void liberar_conexion(int socket_cliente);
void *sacar_de_buffer(t_buffer *buffer, int tam_dato);
void agregar_a_buffer(t_buffer* buffer, void* valor, int bytes);
void eliminar_paquete(t_paquete *paquete);

//SERIALIZACION Y DESERIALIZACION DE MENSAJES ENTRE MODULOS
void serializar_pcb(t_paquete *paquete, t_pcb *pcb);
void serializar_contexto(t_paquete *paquete, t_contexto* contexto);
t_contexto* deserializar_contexto(int socket);
void enviarContexto(t_contexto* contexto_ejecucion,int op_code, int socket);
void serializar_peticion_instruccion(t_paquete* paquete, t_peticion_instruccion* pedir_instruccion);  //Pedir a memoria sgte instrucción
t_peticion_instruccion* deserializar_peticion_instruccion(int socket);  //En memoria recibir esta estructura
void serializar_proceso_nuevo(t_paquete* paquete, t_memoria_instruccion* proceso_nuevo);  //Enviar a memoria datos para estructura de instrucciones
t_memoria_instruccion* deserializar_proceso_nuevo(int socket);  //En memoria recibir para crear estructura para pet. de instrucciones
char* recibir_valor(int socket_cliente);





#endif /* CONEXION_H_ */
