/*
 * plugin_manager.c
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#include <apr-1/apr_queue.h>
#include <stdlib.h>

#include "plugin_manager.h"

typedef struct PluginHookList_t {
	PluginHook hook;
	struct PluginHookList_t* next;
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

void PluginManager_register_hook(PluginManager *pm, PluginHook hook) {
	PluginHookList *node = malloc(sizeof(PluginHookList));
	node->hook = hook;
	node->next = pm->hook_list;
	pm->hook_list = node;
}

metric_t PluginManager_apply_hooks(PluginManager *pm) {
	metric_t retMetric;

	PluginHookList *plugin = pm->hook_list;
	if (!plugin)
		return retMetric;
	while (plugin) {
		retMetric = plugin->hook();
	}
	plugin = plugin->next;

	return retMetric;
}
