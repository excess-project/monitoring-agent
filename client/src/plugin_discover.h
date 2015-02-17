/*
 * plugin_discover.h
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#ifndef PLUGIN_DISCOVER_H_
#define PLUGIN_DISCOVER_H_

#include "plugin_manager.h"

/** @brief number of found plugins
 *
 */
extern int pluginCount;

extern char* plugin_name[256];

/** @brief searches for plugins
 *
 */
void* discover_plugins(const char *dirname, PluginManager *pm);

/** @brief parses the plugin name from the name of the .so file
 *
 */
char* get_plugin_name(char filename[256]);

/** @brief cleanup
 *
 */
void cleanup_plugins(void*);

#endif /* PLUGIN_DISCOVER_H_ */
