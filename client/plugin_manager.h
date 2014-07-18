/*
 * plugin_manager.h
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#ifndef PLUGIN_MANAGER_H_
#define PLUGIN_MANAGER_H_

#include "util.h"

typedef metric_t (*PluginHook)();

typedef struct PluginManager_t PluginManager;

PluginManager* PluginManager_new();

void PluginManager_free(PluginManager *pm);

void PluginManager_register_hook(PluginManager *pm, PluginHook hook);

metric_t PluginManager_apply_hook(PluginManager *pm);

#endif /* PLUGIN_MANAGER_H_ */
