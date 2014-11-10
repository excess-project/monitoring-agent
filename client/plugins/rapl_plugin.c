#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rapl_plugin.h"

void initialize_PAPI()
{
    int retval = PAPI_is_initialized();

    if (retval != PAPI_LOW_LEVEL_INITED) {
        int retval = PAPI_library_init(PAPI_VER_CURRENT);
        if (retval != PAPI_VER_CURRENT) {
            // handle error
            printf("%s", "RAPL: wrong PAPI version");
        }
    }
}


int get_available_events(RAPL_Plugin *rapl)
{
    int EventSet = PAPI_NULL;
    int num_events = 0;

    //initialize_PAPI();

    int retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK) {
        // handle error
        return -1;
    }

    int code = PAPI_NATIVE_MASK;
    int rapl_cid = get_rapl_component_id();
    int r = PAPI_enum_cmp_event(&code, PAPI_ENUM_FIRST, rapl_cid);
    PAPI_event_info_t evinfo;

    while (r == PAPI_OK) {
        retval = PAPI_event_code_to_name(code, rapl->events[num_events]);
        //printf("event_name: %s\n", rapl->events[num_events]);
        if (retval != PAPI_OK) {
            printf("Error translating %#x\n", code);
            return -1;
        }

        retval = PAPI_get_event_info(code, &evinfo);
        if (retval != PAPI_OK) {
            printf("Error getting event info: %d\n", retval);
            return -1;
        }

        //strncpy(units[num_events], evinfo.units, PAPI_MIN_STR_LEN);
        rapl->data_types[num_events] = evinfo.data_type;

        retval = PAPI_add_event(EventSet, code);
        if (retval != PAPI_OK) {
            break; /* We've hit an event limit */
        }
        num_events++;

        r = PAPI_enum_cmp_event(&code, PAPI_ENUM_EVENTS, rapl_cid);
    }

    rapl->values = calloc(num_events, sizeof(long long));
    if (rapl->values == NULL) {
        // handle error
        return -1;
    }
    rapl->num_events = num_events;

    retval = PAPI_start(EventSet);
    if (retval != PAPI_OK) {
        // handle error
        return -1;
    }

    /* Run test */
    //sleep(1);

    retval = PAPI_stop(EventSet, rapl->values);
    if (retval != PAPI_OK) {
        // handle error
        return -1;
    }

    retval = PAPI_cleanup_eventset(EventSet);
    if (retval != PAPI_OK) {
        // handle error
        return -1;
    }

    retval = PAPI_destroy_eventset(&EventSet);
    if (retval != PAPI_OK) {
        // handle error
        return -1;
    }

    //PAPI_shutdown();

    return num_events;
}


int get_rapl_component_id()
{
    //initialize_PAPI();

    int cid;
    int rapl_cid;
    const PAPI_component_info_t *cmpinfo = NULL;

    int numcmp = PAPI_num_components();
    for (cid = 0; cid < numcmp; ++cid) {
        if ((cmpinfo = PAPI_get_component_info(cid)) == NULL) {
            // handle error
            printf("%s", "RAPL: cannot call component info");
            return 0;
        }

        if (strstr(cmpinfo->name, "rapl")) {
            rapl_cid = cid;

            if (cmpinfo->disabled) {
                // RAPL component is disabled
                printf("%s", "RAPL: component is disabled");
                return 0;
            }
            break;
        }
    }

    if (cid == numcmp) {
        // No RAPL component found
        printf("%s", "RAPL: component not found");
        return 0;
    }

    return rapl_cid;
}
