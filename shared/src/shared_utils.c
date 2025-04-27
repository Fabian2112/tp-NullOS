/*
 * shared_utils.c
 *
 *  Created on: Sep 14, 2023
 *      Author: utnso
 */

#include "shared_utils.h"

char *mi_funcion_compartida()
{
    printf("Hice uso de la shared!");
    return 0;
}


int handshakeCliente(int socket_cliente, int modulo)
{
    uint32_t handshake = modulo;
    uint32_t result = -1;
    send(socket_cliente, &handshake, sizeof(uint32_t), 0);
    recv(socket_cliente, &result, sizeof(uint32_t), MSG_WAITALL);

    return result;
}

int handshakeServidor(int socket_cliente)
{
    uint32_t handshake;
    uint32_t resultOk = 0;
    //uint32_t resultError = -1;

    recv(socket_cliente, &handshake, sizeof(uint32_t), MSG_WAITALL);
    sleep(3);

    send(socket_cliente, &resultOk, sizeof(uint32_t), 0);
    printf("Handshake con %s\n", op_ModuloToString(handshake));
    return handshake;
}

const char *op_ModuloToString(int opCode)
{

    {
        switch (opCode)
        {
        case CPU:
             return "CPU";
        case FILESYSTEM:
                    return "FILESYSTEM";
        case KERNEL:
            return "KERNEL";
        case MEMORIA:
            return "MEMORIA";
        default:
            return "Error";
        }
    }
}

t_monitor* monitor_create(){
	t_monitor* monitor = malloc(sizeof(t_monitor));
	monitor->cola = list_create();
	pthread_mutex_init(&(monitor->mutex_cola), NULL);
	return monitor;
}

void monitor_destroy(t_monitor* monitor){
	list_destroy(monitor->cola);
	pthread_mutex_destroy(&(monitor->mutex_cola));
	free(monitor);
}
