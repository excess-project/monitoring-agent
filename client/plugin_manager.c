/*
 * plugin_manager.c
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#include <stdlib.h>

#include "plugin_manager.h"
#include "plugin_discover.h"

typedef struct PluginHookList_t {
	PluginHook hook;
	struct PluginHookList_t* next;
	const char *name;
} PluginHookList;

struct PluginManager_t {
	PluginHookList* hook_list;
};

PluginManager* PluginManager_new() {
	PluginManager *pm = malloc(sizeof(PluginManager));
	pm->hook_list = NULL;
	return pm;
}

void PluginManager_free(PluginManager *pm) {
	PluginHookList* plugin = pm->hook_list;
	while (plugin) {
		PluginHookList *next = plugin->next;
		free(plugin->next);
		free(plugin);
		plugin = next;
	}
	free(pm);
}

void PluginManager_register_hook(PluginManager *pm, const char *name,
		PluginHook hook) {
	PluginHookList *node = malloc(sizeof(PluginHookList));
	node->hook = hook;
	node->name = name;
	node->next = pm->hook_list;
	pm->hook_list = node;
	pluginCount++;
}
// Don't actually need this funtion use
metric PluginManager_apply_hook(PluginManager *pm) {
	metric retMetric = malloc(sizeof(*retMetric));

	PluginHookList *plugin = pm->hook_list;
	if (!plugin) {
		free(retMetric);
		return NULL ;
	}
	while (plugin) {
		retMetric = plugin->hook();

		plugin = plugin->next;
	}

	return retMetric;
}

PluginHook PluginManager_get_hook(PluginManager *pm) {
	PluginHook funcPtr = pm->hook_list->hook;
	fprintf(stdout, "\nusing plugin %s ", pm->hook_list->name);
	pm->hook_list = pm->hook_list->next;

	return funcPtr;
}
