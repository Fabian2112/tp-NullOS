/*
 * planificacion.c
 *
 *  Created on: Oct 19, 2023
 *      Author: utnso
 */

#include "../include/planificacion.h"

void inicializar_variables(){
	id_proceso =  1;

	gradoMultiprogramacion = grado_multiprogramacion();

	setear_algoritmo();

	if(algoritmo == PRIORIDADES)
		desalojo = 1;
	else
		desalojo = 0;
}


void setear_algoritmo(){
	if(strcmp(algoritmo_planificacion(), "FIFO") == 0){
		algoritmo = FIFO;
	}
	else if (strcmp(algoritmo_planificacion(), "RR") == 0){
		algoritmo = RR;
	}
	else if(strcmp(algoritmo_planificacion(), "PRIORIDADES") == 0){
		algoritmo = PRIORIDADES;
	}
	else{
		log_error(logger, "Algoritmo de planificacion desconocido");
	}
}


void iniciar_estructuras(){
	// MONITORES: COLA + MUTEX
	cola_general = monitor_create();
	colas_new = monitor_create();
	cola_ready = monitor_create();
	cola_exec = monitor_create();
	cola_block = monitor_create();
	cola_unlock = monitor_create();
	cola_exit = monitor_create();

	// SEMAFOROS

	pthread_mutex_init(&mutexIdProceso, NULL);
	pthread_mutex_init(&mutexGradoMultiprogramacion, NULL);
	pthread_mutex_init(&mutex_page_fault, NULL);
	sem_init(&pendientes_new, 0, 0);
	sem_init(&pendientes_ready, 0, 0);
	sem_init(&pendientes_unlock, 0, 0);
	sem_init(&pendientes_exit, 0, 0);
	sem_init(&sem_multiprogramacion, 0, gradoMultiprogramacion);
	sem_init(&sem_quantum, 0, 0);
	sem_init(&cpu_libre, 0, 1);
	sem_init(&detenerPlanificacion,0,1);
	sem_init(&nuevos_new, 0, 0);
}


void liberar_estructuras(){
	// COLAS
	monitor_destroy(cola_general);
	monitor_destroy(colas_new);
	monitor_destroy(cola_ready);
	monitor_destroy(cola_exec);
	monitor_destroy(cola_block);
	monitor_destroy(cola_unlock);
	monitor_destroy(cola_exit);

	// CHECK: habria que eliminar los PCBs que (capaz) contienen tambien??

	// SEMAFOROS
	pthread_mutex_destroy(&mutexIdProceso);
	pthread_mutex_destroy(&mutexGradoMultiprogramacion);
	pthread_mutex_destroy(&mutex_page_fault);
	sem_destroy(&pendientes_new);
	sem_destroy(&pendientes_ready);
	sem_destroy(&pendientes_unlock);
	sem_destroy(&pendientes_exit);
	sem_destroy(&sem_multiprogramacion);
	sem_destroy(&sem_quantum);
	sem_destroy(&cpu_libre);
	sem_destroy(&detenerPlanificacion);
	sem_destroy(&nuevos_new);
}

void planificar(){
	planificador_largo_plazo();
	planificador_corto_plazo();
}

/* 	===============================================================================================================
					PLANIFICADOR LARGO PLAZO
	===============================================================================================================*/

void agregarNEW(t_pcb *pcb){

	//log_info(logger, "Cant. new antes %d ", list_size(colas_new->cola));

	list_add_con_mutex(cola_general, pcb);
	list_add_con_mutex(colas_new, pcb);

	pcb->estado = NUEVO;

	//log_info(logger, "Cant. new post %d ", list_size(colas_new->cola));

	log_info(logger, "Se crea el proceso < %d > en NEW", pcb->contexto->pid);

	  if (enviar_memoria_instrucciones(pcb) == false){
		  finalizar_new_exit(pcb);
	  }

	 log_info(logger,"Enviar iniciar_proceso a memoria");
	 enviar_crear_proceso(pcb);

	//sem_post(&pendientes_new);
	sem_post(&nuevos_new);  //para validar si no se finalizó el proceso estando en new
}


void planificador_largo_plazo(){
	pthread_t hilo_new_a_ready;
	pthread_t hilo_exit;
	/* DECISION: realmente no es necesario un hilo de exit, se podria ejecutar una funcion terminar_proceso(pcb) cuando
	   se necesite, pero para no darle muchas responsabilidades a un solo hilo esta bueno.*/
	pthread_t hilo_sigue_en_new;

	pthread_create(&hilo_new_a_ready, NULL, (void *)new_a_ready_PCB, NULL);
	pthread_create(&hilo_exit, NULL, (void *)exit_PCB, NULL);
	pthread_create(&hilo_sigue_en_new, NULL, (void *)sigueEnNew, NULL);

	pthread_detach(hilo_new_a_ready);
	pthread_detach(hilo_exit);
	pthread_detach(hilo_sigue_en_new);
}


void sigueEnNew(){
  while(true){

	sem_wait(&nuevos_new);

	sem_wait(&sem_multiprogramacion); // CAMBIO

	sem_wait(&detenerPlanificacion);


	int cantidad= list_size(colas_new->cola); //log_info(logger, "Cant. en New  %d", cantidad);

     if (cantidad > 0){

    	  sem_post(&pendientes_new);
	}

     sem_post(&detenerPlanificacion);

   }
}


void new_a_ready_PCB(){
	while(true){
		sem_wait(&pendientes_new);
		sem_wait(&detenerPlanificacion);

		t_pcb* pcb = list_remove_con_mutex(colas_new);
	   	//sem_wait(&sem_multiprogramacion);

	   	//log_info(logger,"Enviar iniciar_proceso a memoria");
	   	//enviar_crear_proceso(pcb);

		int valor;
		sem_getvalue(&sem_multiprogramacion, &valor);
		transicion_a_ready(pcb);
		log_info(logger,"Valor de multiprogramación %i", valor);

		sem_post(&pendientes_ready);
		sem_post(&detenerPlanificacion);
	}
}


void exit_PCB(){
	while(true){
		sem_wait(&pendientes_exit);
		t_pcb* pcb = list_remove_con_mutex(cola_exit);
		list_remove_pcb_con_mutex(cola_general, pcb);
		log_info(logger, "Finaliza el proceso %d - Motivo: %s", pcb->contexto->pid, motivoToString(pcb->motivo_fin));

		liberar_recursos_pcb(pcb);
		log_info(logger, "RECURSOS de PID: %d liberados correctamente", pcb->contexto->pid);

		enviar_eliminar_proceso(pcb);   // MEMORIA

		destruirPCB(pcb);

		sem_post(&sem_multiprogramacion);
	}
}


void transicion_exec_a_exit(t_pcb* pcb){
	sem_wait(&detenerPlanificacion);

	cambiar_estado(pcb, FINALIZADO);
	list_add_con_mutex(cola_exit, pcb);

	sem_post(&detenerPlanificacion);

	sem_post(&pendientes_exit);
	sem_post(&cpu_libre);
}

/* 	===============================================================================================================
						PLANIFICADOR CORTO PLAZO
	===============================================================================================================*/

void planificador_corto_plazo(){
	pthread_t hilo_ready_a_exec;
	pthread_t hilo_block_a_ready; // Teoricamente este hilo no va en el planificador de corto plazo, pero lo pongo aca por temas de orden (no se donde ponerlo jaja)

	pthread_create(&hilo_ready_a_exec, NULL, (void *)ready_a_exec_PCB, NULL);
	pthread_create(&hilo_block_a_ready, NULL, (void *)block_a_ready_PCB, NULL);


	pthread_detach(hilo_ready_a_exec);
	pthread_detach(hilo_block_a_ready);


}


void ready_a_exec_PCB(){
	while(true){
		sem_wait(&cpu_libre);
		sem_wait(&pendientes_ready);
		sem_wait(&detenerPlanificacion); //log_info(logger, "detener ready a exec");
		sem_post(&detenerPlanificacion);

		t_pcb* pcb = elegir_proximo_proceso();
		dispatcher(pcb);
	}
}

void exec_a_ready_PCB(t_pcb* pcb){  //desalojo
  sem_wait(&detenerPlanificacion);
  list_add_con_mutex(cola_ready, pcb);
  cambiar_estado(pcb, LISTO);

  sem_post(&detenerPlanificacion);
  sem_post(&pendientes_ready);
  sem_post(&cpu_libre);
}

t_pcb* elegir_proximo_proceso(){

	t_pcb* pcb;

	switch(algoritmo) {

	case FIFO:
		pcb = list_remove_con_mutex(cola_ready);
		break;

	case RR:
		pcb = list_remove_con_mutex(cola_ready);

		pthread_t hilo_quantum;
		pthread_create(&hilo_quantum, NULL, (void *)esperar_quantum, &pcb);
		pthread_detach(hilo_quantum);

		break;

	case PRIORIDADES:
		pthread_mutex_lock(&(cola_ready->mutex_cola));
		pcb = list_get_minimum(cola_ready->cola, (void*)comparar_prioridad);
		log_info(logger,"PID : %d  con prioridad alta= %d",pcb->contexto->pid, pcb->prioridad );
		list_remove_element(cola_ready->cola, pcb);
		pthread_mutex_unlock(&(cola_ready->mutex_cola));
		break;

	default:
		log_error(logger, "Algoritmo de planificacion desconocido");
		abort();
	}

	return pcb;
}


void esperar_quantum(){
	sem_wait(&sem_quantum);
	usleep(quantum() * 1000);
    int cantidad=  list_size(cola_exec->cola );
	if(cantidad > 0){
		t_pcb* pcb_exec =  list_get(cola_exec->cola,0);

		enviarContexto(pcb_exec->contexto,_DESALOJO, socket_interrupt);
		log_info(logger, "PID: %d - Desalojado por fin de Quantum", pcb_exec->contexto->pid);
	}
}

void dispatcher(t_pcb* pcb){

	list_add_con_mutex(cola_exec, pcb);
	cambiar_estado(pcb, EJECUTANDO);
	if(algoritmo == RR)
		sem_post(&sem_quantum);

    enviarContexto(pcb->contexto,_CONTEXTO, socket_dispatch);

}


void block_a_ready_PCB(){
	while(true){
		sem_wait(&pendientes_unlock);
		sem_wait(&detenerPlanificacion);
		sem_post(&detenerPlanificacion);

		pthread_mutex_lock(&(cola_unlock->mutex_cola));
		int cant_procesos_unlock = list_size(cola_unlock->cola);
		pthread_mutex_unlock(&(cola_unlock->mutex_cola));

		if(cant_procesos_unlock > 0){
			t_pcb* pcb = list_remove_con_mutex(cola_unlock);
			transicion_a_ready(pcb);
			sem_post(&pendientes_ready);
		}

	}
}

void transicion_a_ready(t_pcb* pcb){
	list_add_con_mutex(cola_ready, pcb);
	cambiar_estado(pcb, LISTO);
	if(desalojo && list_size(cola_exec->cola) > 0){
		t_pcb* pcb_exec = list_get(cola_exec->cola, 0);
		enviarContexto(pcb_exec->contexto,_DESALOJO, socket_interrupt);
		log_info(logger, "PID: %d - Desalojado por ingreso a READY del PID: %d", pcb_exec->contexto->pid, pcb->contexto->pid);
		// send_interrupt(DESALOJO, socket_interrupt)
		//enviarContexto(pcb->contexto,_DESALOJO, socket_interrupt);
	}
}

/* 	===============================================================================================================
						PCB
	===============================================================================================================*/

t_pcb* crearPCB(int size, int prioridad, char* nombre_archivo){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	t_contexto* contexto = malloc(sizeof(t_contexto));
    contexto->registros = malloc(sizeof(t_registros));

	pcb->prioridad= prioridad;
	pcb->size = size;
	pcb->nombre_txt = nombre_archivo;
	pcb->contexto = contexto;
	pcb->file_opens = list_create();


	contexto->registros->AX =0;
	contexto->registros->BX= 0;
	contexto->registros->CX= 0;
	contexto->registros->DX= 0;
	//pcb->contexto->registros = registros;


	pthread_mutex_lock(&mutexIdProceso);
	pcb->contexto->pid  = id_proceso++;
	pthread_mutex_unlock(&mutexIdProceso);
	pcb->contexto->program_counter = 0;

    return pcb;
}


void destruirPCB(t_pcb* pcb){
	liberarContexto(pcb->contexto);
	list_destroy(pcb->file_opens);
	free(pcb);
}

/* 	===============================================================================================================
						UTILES
	===============================================================================================================*/

void cambiar_estado(t_pcb* pcb, code_estado nuevo_estado){
	const char* estado_anterior =   estadoToString(pcb->estado);
	const char* estado_actual =   estadoToString(nuevo_estado);

	log_info(logger, "PID: %d - Estado Anterior: %s - Estado Actual: %s", pcb->contexto->pid, estado_anterior, estado_actual);
	pcb->estado = nuevo_estado;
}


void list_add_con_mutex(t_monitor* monitor, void* data){
	pthread_mutex_lock(&(monitor->mutex_cola));
	list_add(monitor->cola, data);
	pthread_mutex_unlock(&(monitor->mutex_cola));
}


void* list_remove_con_mutex(t_monitor* monitor){
	void* data;
	pthread_mutex_lock(&(monitor->mutex_cola));
	data = list_remove(monitor->cola, 0); // index = 0 --> remueve el primer elemento
	pthread_mutex_unlock(&(monitor->mutex_cola));
	return data;
}


void list_remove_pcb_con_mutex(t_monitor* monitor, t_pcb* pcb_proceso){
	bool removido;
	pthread_mutex_lock(&(monitor->mutex_cola));

	removido= list_remove_element(monitor->cola, pcb_proceso);
	if(removido == true){
		//log_info(logger, "Se removió el pcb de la cola");
	}
	else{
		log_info(logger, "No se pudo remover el pcb de la cola");
	}

	pthread_mutex_unlock(&(monitor->mutex_cola));
}


t_pcb* comparar_prioridad(t_pcb* proceso_1, t_pcb* proceso_2){
	t_pcb* proceso_prioritario;

	if(proceso_1->prioridad <= proceso_2->prioridad)	// MENOR NUMERO ==> MAYOR PRIORIDAD
		proceso_prioritario = proceso_1;
	else
		proceso_prioritario = proceso_2;

	return proceso_prioritario;
}

//Cuando pase de new a exec aparte de generar en memoria estructa también completar dato en memoria de instrucciones con datos del pcb(archivo, pid)


/* 	===============================================================================================================
						UTILES PARA DETENER PLANIFICACION
	===============================================================================================================*/


void finalizar_new_exit(t_pcb* pcb){

	pasar_a_exit(colas_new, pcb);
	pcb = list_remove_con_mutex(cola_exit);  // con semáforo no, ya que de new a exit no se necesita incrementar el semáforo de multiprogramación
	log_info(logger, "Finaliza el proceso %d - Motivo: %s", pcb->contexto->pid, motivoToString(pcb->motivo_fin));
	enviar_eliminar_proceso(pcb);

	destruirPCB(pcb);
	pendientes_new.__align = pendientes_new.__align - 1;
}


void finalizar_ready_exit(t_pcb* pcb){

	pasar_a_exit(cola_ready, pcb);

	pendientes_ready.__align = pendientes_ready.__align - 1;  //decremento por que sino el wait de este proceso trata de ejecutar un pcb vacío

	sem_post(&pendientes_exit);


}


void finalizar_exec_exit(t_pcb* pcb){

	//CPU ENVIA POR INTERRUPCIÓN DE FINALIZAR PROCESO, SALIDA POR EXIT!!!, KERNEL_DISPATCH MANEJA LA TRANSICION DE EXEC_EXIT
	bool es_pid(void *_pcbExec){
		t_pcb* pcbExec = (t_pcb *)_pcbExec;
		return pcbExec->contexto->pid == pcb->contexto->pid;
	}

	t_pcb* pcbExec = list_find(cola_exec->cola, &es_pid);

	if(pcbExec != NULL){
		enviarContexto(pcb->contexto,_FINALIZAR_PROCESO, socket_interrupt);
		log_info(logger, "PID: %d - Interrumpir por fin de proceso", pcb->contexto->pid);
	}

}


void finalizar_block_exit(t_pcb* pcb){
	pasar_a_exit(cola_block, pcb);
	sem_post(&pendientes_exit);
}


void finalizar_unlock_exit(t_pcb* pcb){
	pasar_a_exit(cola_unlock, pcb);
	sem_post(&pendientes_exit);
}

void pasar_a_exit(t_monitor* monitor, t_pcb* pcb){

	list_remove_pcb_con_mutex(monitor, pcb);
	pcb->motivo_fin = SUCCESS ;
	cambiar_estado(pcb, FINALIZADO);
	list_add_con_mutex(cola_exit, pcb);
}

/* 	===============================================================================================================
						PARA COMUNICACION DE MODULOS
	===============================================================================================================*/

void enviar_crear_proceso(t_pcb* pcb){

    log_info(logger, "Solicitud a MEMORIA: INICIAR PROCESO con PID: %d", pcb->contexto->pid);

	int tamanio_nombre = strlen(pcb->nombre_txt) + 1;

	t_paquete* paquete = crear_paquete(_INICIAR_PROCESO);
	//agregar_a_paquete(paquete, &tamanio_nombre, sizeof(int));
	agregar_a_paquete(paquete, (pcb->nombre_txt), tamanio_nombre);
	agregar_a_paquete(paquete, &(pcb->size), sizeof(int));
	agregar_a_paquete(paquete, &(pcb->contexto->pid), sizeof(int));
	enviar_paquete(paquete, socket_memoria);


	char* respuesta = recibir_valor(socket_memoria);
	free(respuesta);
	//log_info(logger,"RESPUESTA INICIAR_PROCESO MEMORIA: %s", respuesta);

}


void enviar_eliminar_proceso(t_pcb* pcb){
	t_paquete* paquete = crear_paquete(_FINALIZAR_PROCESO);
	agregar_a_paquete(paquete, &(pcb->contexto->pid), sizeof(int));
	enviar_paquete(paquete, socket_memoria);

	//char* respuesta = recibir_valor(socket_memoria);	// CAPAZ NO DEBERIA BLOQUEARSE
}


bool enviar_memoria_instrucciones(t_pcb* pcb){
	//enviar al kernel para memoria de instrucciones

	t_memoria_instruccion* proceso_nuevo =   malloc (sizeof(t_memoria_instruccion));
	 proceso_nuevo->pid= pcb->contexto->pid;
	 proceso_nuevo->nombre_archivo = pcb->nombre_txt;
	 t_paquete* paquete= crear_paquete(_MEMORIA_INSTRUCCIONES);
	 log_info(logger,"Envio pid: %d  y nombre de archivo: %s a memoria de instrucciones ", proceso_nuevo->pid, proceso_nuevo->nombre_archivo );

	 serializar_proceso_nuevo(paquete, proceso_nuevo);
     enviar_paquete(paquete, socket_memoria);

     char* mensaje = recibir_valor(socket_memoria);

     bool creado_ok= false ;
     if (string_equals_ignore_case(mensaje,"ERROR")) {
    	 log_info(logger, "No se generaron las instrucciones correctamente");
     }
     else{
    	 log_info(logger, "Memoria de instrucciones ok");
    	 creado_ok = true;
     }

     liberarProcesoNuevo(proceso_nuevo);
     free(mensaje);
     return creado_ok;
}

