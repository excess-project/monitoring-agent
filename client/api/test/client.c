#include <stdlib.h>

#include "mf_api.h"

int
main(int argc, char** argv)
{
    char* conf_file = "../../../mf_config.ini";

    mf_api_initialize(conf_file);

    mf_api_start_profiling("fcnt1");

    // do something in between
    // e.g. fcnt1(void);

    mf_api_stop_profiling("fcnt1");

    // query database for range of time for all metrics
    /*
    char *res = queryRangeFromDB("http://mf.excess-project.eu",
            "JcJNPcVZRa6bVSnn9rejcA", 1411648702.428327942,
            1411648702.429081288);
    */

    //query database for stats of a metric
    /*
    char *res1 = querySpecificStatFromDB("http://mf.excess-project.eu",
            "JcJNPcVZRa6bVSnn9rejcA", 1411648702.428327942,
            1411648702.429081288, "mem_used");
    */

    return EXIT_SUCCESS;
}