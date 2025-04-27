/*
 * log_shared.c
 *
 *  Created on: Sep 16, 2023
 *      Author: utnso
 */

#include "log_shared.h"

t_log *LogInit(const char *file, const char *program, bool isActiveConsole)
{
    t_log *nuevo_logger;

    if ((nuevo_logger = log_create((char *)file, (char *)program, isActiveConsole, LOG_LEVEL_INFO)) == NULL)
    {
        printf("No se creó el log \n");
        exit(1);
    }
    return nuevo_logger;
}

void LogTerminate(t_log *logger)
{
    if (logger != NULL)
        log_destroy(logger);
}

