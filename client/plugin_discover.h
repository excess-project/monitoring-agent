/*
 * plugin_discover.h
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#ifndef PLUGIN_DISCOVER_H_
#define PLUGIN_DISCOVER_H_

#include "plugin_manager.h"

extern int pluginCount;

void* discover_plugins(const char *dirname, PluginManager *pm);

char* get_plugin_name(char filename[256]);

void cleanup_plugins(void*);

#endif /* PLUGIN_DISCOVER_H_ */
