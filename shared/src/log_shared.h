/*
 * log_shared.h
 *
 *  Created on: Sep 16, 2023
 *      Author: utnso
 */

#ifndef LOG_SHARED_H_
#define LOG_SHARED_H_

#include "TypeDef.h"

/**
 * @NAME: LogInit
 * @DESC: Inicializa el logger de la libreria.
 * @PARAMS:
 *          [in] const char* file - Nombre del archivo al cual se va a logear.
 *          [in] const char* program - Nombre del programa.
 *          [in] bool isActiveConsole - Si se va a logear a consola o no.
 */
t_log *LogInit(const char *file, const char *program, bool isActiveConsole);

/**
 * @NAME: LogTerminate
 * @PARAMS:
 *          [t_log*] - Logger a finalizar.
 * @DESC: Finaliza el logger de la libreria.
 */
void LogTerminate(t_log *logger);



#endif /* LOG_SHARED_H_ */
