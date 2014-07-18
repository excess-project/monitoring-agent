/*
 * plugin_discover.c
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <dlfcn.h>

#include "plugin_discover.h"

typedef struct PluginHandleList_t {
	void* handle;
	struct PluginHandleList_t* next;
} PluginHandleList;

typedef int (*PluginInitFunc)(PluginManager *pm);

void* load_plugin(char *name, char *fullpath, PluginManager *pm) {
	char *slashed_path = malloc(strlen(fullpath) + 1);

	void *libhandle = dlopen(slashed_path, RTLD_NOW);

	if (!libhandle) {
		fprintf(stderr, "Error loading library: %s\n,", dlerror());
	}

	char *init_func_name = malloc(strlen("init_") + strlen(name));
	strcat(init_func_name, name);

	PluginInitFunc init_func = (PluginInitFunc) (intptr_t) dlsym(libhandle,
			init_func_name);

	if (!init_func) {
		fprintf(stderr, "Error loadding init function: %s\n", dlerror());
	}

	int rc = init_func(pm);

	if (rc < 0) {
		fprintf(stderr, "Error: Plugin init function returned %d\n", rc);
		dlclose(libhandle);
	}
	return libhandle;
}

int discover_plugins(const char *dirname, PluginManager *pm) {
	DIR* dir = opendir(dirname);

	if (!dir) {
		fprintf(stderr, "unable to open directory %s!\n", dirname);
		return 0;
	}

	struct dirent *direntry;
	while ((direntry = readdir(dir))) {
		char *name = get_plugin_name(direntry->d_name);
		if (!name)
			continue;
		char *fullpath = strcpy(fullpath, dirname);
		strcat(fullpath, "/");
		strcat(fullpath, direntry->d_name);
		void *handle = load_plugin(name, fullpath, pm);
		if (handle) {
			;
		}
	}

	closedir(dir);

	return 0;
}

char* get_plugin_name(char filename[256]) {
	char *retStr;
	retStr = '\0';

	char *last_slash = strrchr(filename, '/');
	char *name_start = last_slash ? last_slash + 1 : filename;
	char *last_dot = strrchr(filename, '.');

	if (!last_dot || strcmp(last_dot, ".so"))
		return NULL ;

	return strncpy(retStr, name_start, last_dot - name_start);
}
