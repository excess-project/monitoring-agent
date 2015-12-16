#include <stdlib.h>

#include "mf_vmstat_connector.h"

int
main(int argc, char** argv)
{
    vmstat_plugin* vmstat = malloc(sizeof(vmstat_plugin));

    --argc;
    mf_vmstat_read(vmstat, argv, argc);
    mf_vmstat_print(vmstat);

    free(vmstat);
}