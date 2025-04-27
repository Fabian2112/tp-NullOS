/*
 * config_shared.c
 *
 *  Created on: Sep 16, 2023
 *      Author: utnso
 */
#include "config_shared.h"

static t_config *ConfigInstance = NULL;

bool ConfigInit(const char *configPath)
{
    if (ConfigInstance != NULL)
        return true;

    ConfigInstance = config_create((char *)configPath);

    return (ConfigInstance != NULL) ? true : false;
}

void ConfigTerminate(void)
{
    if (ConfigInstance != NULL)
        config_destroy(ConfigInstance);
}

const char *ConfigGetStringValue(const char *key)
{
    return config_get_string_value(ConfigInstance, (char *)key);
}

int ConfigGetIntValue(const char *key)
{
    return config_get_int_value(ConfigInstance, (char *)key);
}



