/*
 * plugin_manager.h
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#ifndef PLUGIN_MANAGER_H_
#define PLUGIN_MANAGER_H_

#include "util.h"

/**
 * @brief entry function of plugin
 */
typedef metric (*PluginHook)();

/**
 * @brief structure which contains the plugins
 */
typedef struct PluginManager_t PluginManager;

/**
 * @brief initializing function for the plugin manager
 */
PluginManager* PluginManager_new();

/**
 * @brief functions which deallocates the memeory used
 */
void PluginManager_free(PluginManager *pm);

/**
 * @brief registers hooks / entry functions of plugins
 */
void PluginManager_register_hook(PluginManager *pm, const char *name,
		PluginHook hook);
/**
 * @brief function which applies an entry function
 * @deprecated { not needed }
 */
metric PluginManager_apply_hook(PluginManager *pm);

/**
 * @brief returns a entry function from the plugin manager structure
 *
 * hooks / entry functions are stored in a FIFO queue
 */
PluginHook PluginManager_get_hook(PluginManager *pm);

#endif /* PLUGIN_MANAGER_H_ */
