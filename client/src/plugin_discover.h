/*
 * Copyright 2014, 2015 High Performance Computing Center, Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
