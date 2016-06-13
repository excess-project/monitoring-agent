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

#include <dlfcn.h>
#include <dirent.h>
#include <mf_parser.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "mf_types.h"
#include "excess_main.h"
#include "plugin_discover.h"
#include "thread_handler.h"

typedef struct PluginHandleList_t {
	void* handle;
	struct PluginHandleList_t* next;
} PluginHandleList;

typedef struct PluginDiscoveryState_t {
	PluginHandleList* handle_list;
} PluginDiscoveryState;

typedef int (*PluginInitFunc)(PluginManager *pm);

int pluginCount = 0;

char* plugin_name[256];

void* load_plugin(char *name, char *fullpath, PluginManager *pm) {
	char* slashed_path = strdup(fullpath);

	void *libhandle = dlopen(slashed_path, RTLD_NOW);

	if (!libhandle) {
		fprintf(stderr, "Error loading library: %s\n", dlerror());
		fprintf(logFile, "Error loading library: %s\n", dlerror());
	}

	char *init_func_name = malloc(
			(strlen("init_") + strlen(name)) * sizeof(char) + 1);

	strcpy(init_func_name, "init_");
	strcat(init_func_name, name);

	void *ptr = dlsym(libhandle, init_func_name);
	free(init_func_name);
	if (!ptr) {
		fprintf(stderr, "Error loading init function: %s\n", dlerror());
		fprintf(logFile, "Error loading init function: %s\n", dlerror());
	}
	PluginInitFunc init_func = (PluginInitFunc) (intptr_t) ptr;

	int rc = init_func(pm);

	if (rc < 0) {
		fprintf(stderr, "Error: Plugin init function returned %d\n", rc);
		fprintf(logFile, "Error: Plugin init function returned %d\n", rc);
		dlclose(libhandle);
		return NULL ;
	}

	return libhandle;
}

void* discover_plugins(const char *dirname, PluginManager *pm) {
	DIR* dir = opendir(dirname);

	if (!dir) {
		fprintf(stderr, "unable to open directory %s!\n", dirname);
		fprintf(logFile, "unable to open directory %s!\n", dirname);
		return NULL ;
	}

	PluginDiscoveryState *plugins_state = malloc(sizeof(*plugins_state));
	plugins_state->handle_list = NULL;

	plugin_name[0] = malloc(sizeof(char) * 256);
	plugin_name[1] = malloc(sizeof(char) * 256);

	struct dirent *direntry;
	while ((direntry = readdir(dir))) {
		char *name = get_plugin_name(direntry->d_name);
		if (!name) {
			continue;
		}

		/* do not consider plug-ins that are switched off */
		char value[20] = {'\0'};
		mfp_get_value("plugins", name, value);
		//char* value = mfp_get_value("plugins", name);
		if (strcmp(value, "off") == 0) {
			continue;
		}

		char *fullpath = malloc(200 * sizeof(char));

		strcpy(fullpath, dirname);
		strcat(fullpath, "/");
		strcat(fullpath, direntry->d_name);
		plugin_name[MIN_THREADS + pluginCount] = malloc(256);
		strcpy(plugin_name[MIN_THREADS + pluginCount], name);
		void *handle = load_plugin(name, fullpath, pm);
		if (handle) {
			PluginHandleList *handle_node = malloc(sizeof(*handle_node));
			handle_node->handle = handle;
			handle_node->next = plugins_state->handle_list;
			plugins_state->handle_list = handle_node;
		}
		free(fullpath);
	}

	closedir(dir);
	if (plugins_state->handle_list)
		return (void*) plugins_state;
	else {
		free(plugins_state);
		return NULL ;
	}
	return 0;
}

char* get_plugin_name(char filename[256]) {
	char *retStr;

	char *last_slash = strrchr(filename, '/');
	char *name_start = last_slash ? last_slash + 1 : filename;
	char *last_dot = strrchr(filename, '.');

	if (!last_dot || strcmp(last_dot, ".so"))
		return NULL ;
	retStr = calloc(last_dot - name_start + 1, sizeof(char));
	return strncpy(retStr, name_start, last_dot - name_start);
}

void cleanup_plugins(void* vpds) {
	PluginDiscoveryState *pds = (PluginDiscoveryState*) vpds;
	PluginHandleList *node = pds->handle_list;
	while (node) {
		PluginHandleList *next = node->next;
		dlclose(node->handle);
		free(node);
		node = next;
	}
	free(pds);
}
