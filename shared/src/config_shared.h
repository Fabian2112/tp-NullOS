/*
 * config_shared.h
 *
 *  Created on: Sep 16, 2023
 *      Author: utnso
 */

#ifndef CONFIG_SHARED_H_
#define CONFIG_SHARED_H_

#include "TypeDef.h"

/**
 * @DESC: Inicializar la instancia de config de la libreria.
 * @PARAMS:
 *          [in] const char* configPath - ruta al archivo de configuracion que se debe cargar.
 * @RETURN: true en caso de que se haya podido cargar el archivo de configuracion. De lo contrario false.
 */
bool ConfigInit(const char *configPath);

/**
 * @NAME: ConfigTerminate
 * @DESC: Finaliza la instancia de config de la libreria.
 */
void ConfigTerminate(void);

/**
 * @NAME: ConfigGetStringValue
 * @DESC: Obtiene el valor en string de la clave.
 * @PARAMS:
 *          [in] const char* key - clave del valor que se quiere obtener.
 * @RETURNS: Un string con el valor de la clave, o NULL en caso que no exista dicha clave.
 */
const char *ConfigGetStringValue(const char *key);

/**
 * @NAME: ConfigGetIntValue
 * @DESC: Obtiene el valor en int de la clave.
 * @PARAMS:
 *          [in] const char* key - clave del valor que se quiere obtener.
 * @RETURNS: Un int con el valor de la clave.
 */
int ConfigGetIntValue(const char *key);




#endif /* CONFIG_SHARED_H_ */
