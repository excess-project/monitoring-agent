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
#include "excess_main.h"

/**
 * @brief old way of storing hooks
 * @depreciated
 */
typedef struct PluginHookType_t {
	PluginHook hook;
	const char *name;
} PluginHookType;
/**
 * @brief definition of plugin manager struct
 */
struct PluginManager_t {
	struct apr_queue_t *hook_queue;
	struct apr_pool_t *data_pool;
};

PluginManager* PluginManager_new() {
	apr_initialize();
	PluginManager *pm = malloc(sizeof(PluginManager));

	apr_pool_create(&pm->data_pool, NULL);
	apr_queue_create(&pm->hook_queue, 256, pm->data_pool);
	return pm;
}

void PluginManager_free(PluginManager *pm) {
	apr_queue_term(pm->hook_queue);
	apr_pool_destroy(pm->data_pool);
	apr_terminate();
	free(pm);
}

void PluginManager_register_hook(PluginManager *pm, const char *name,
		PluginHook hook) {

	PluginHookType *hookType = malloc(sizeof(PluginHookType));
	hookType->hook = hook;
	hookType->name = name;
	apr_status_t status = apr_queue_push(pm->hook_queue, hookType);
	if (status != APR_SUCCESS) {
		fprintf(stderr, "Failed queue push");
		fprintf(logFile, "Failed queue push");
	}
	pluginCount++;
}

// Don't actually need this funtion
metric PluginManager_apply_hook(PluginManager *pm) {
	metric retMetric = malloc(sizeof(metric_t));

	return retMetric;
}

PluginHook PluginManager_get_hook(PluginManager *pm) {
	PluginHook funcPtr = NULL;
	void *retPtr;
	apr_status_t status = apr_queue_pop(pm->hook_queue, &retPtr);
	if (status == APR_SUCCESS) {
		PluginHookType *typePtr;
		intptr_t hlpr = (intptr_t) retPtr;
		typePtr = (struct PluginHookType_t*) hlpr;
		funcPtr = *(typePtr->hook);
		fprintf(stderr, "using Plugin %s ", typePtr->name);
		fprintf(logFile, "using Plugin %s ", typePtr->name);
	}
	return funcPtr;
}
