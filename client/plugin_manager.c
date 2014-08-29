/*
 * plugin_manager.c
 *
 *  Created on: 16.07.2014
 *      Author: hpcneich
 */

#include <stdlib.h>
#include <pthread.h>
#include <apr_queue.h>

#include "plugin_manager.h"
#include "plugin_discover.h"

typedef struct PluginHookList_t {
	PluginHook hook;
	struct PluginHookList_t* next;
	const char *name;
} PluginHookList;

typedef struct PluginHookType_t {
	PluginHook hook;
	const char *name;
} PluginHookType;

struct PluginManager_t {
	PluginHookList* hook_list;
	struct apr_queue_t *hook_queue;
	struct apr_pool_t *data_pool;
};

PluginManager* PluginManager_new() {
	apr_initialize();
	PluginManager *pm = malloc(sizeof(PluginManager));
	pm->hook_list = NULL;

	apr_pool_create(&pm->data_pool, NULL);
	apr_queue_create(&pm->hook_queue, 256, pm->data_pool);
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

	PluginHookType *hookType = malloc(sizeof(PluginHookType));
	hookType->hook = hook;
	hookType->name = name;
	apr_status_t status = apr_queue_push(pm->hook_queue, hookType);
	if (status != APR_SUCCESS)
		fprintf(stderr, "Failed queue push");

	pluginCount++;
}

// Don't actually need this funtion
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
	fprintf(stdout, "\n using plugin %s ", pm->hook_list->name);

	pm->hook_list = pm->hook_list->next;


	void *retPtr;
	apr_status_t status = apr_queue_pop(pm->hook_queue, &retPtr);
	if (status == APR_SUCCESS) {
		PluginHookType *typePtr;
		intptr_t hlpr = (intptr_t) retPtr;
		typePtr = (struct PluginHookType_t*) hlpr;
		funcPtr = *(typePtr->hook);
	}
	return funcPtr;
}
