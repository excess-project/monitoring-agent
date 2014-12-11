#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mf_api.h"

int
main(int argc, char** argv)
{
    int c;
    const char* URL = NULL;
    char* db_key = NULL;
    long double start_time, end_time;

    opterr = 0;
    while ((c = getopt(argc, argv, "?k:u:")) != -1) {
        switch (c) {
            case 'k':
                db_key = optarg;
                break;
            case 'u':
                URL = optarg;
                break;
            case '?':
                if (optopt == 'k' || optopt == 'u') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint (optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                opterr = 1;
                return(EXIT_FAILURE);
            default:
                fprintf(stderr, "Please specify some options");
                opterr = 1;
                abort();
        }
    }

    if (URL == NULL) {
        fprintf(stderr, "URL not set. Please specify the URL using the 'u' parameter.\n");
        exit(EXIT_FAILURE);
    }
    if (db_key == NULL) {
        fprintf(stderr, "db_key not set. Please specify the key using the 'k' parameter.\n");
        exit(EXIT_FAILURE);
    }

    mf_api_initialize(URL, db_key);

    start_time = mf_api_start_profiling("fcnt1");
    printf("\nStart: %.9Lf\n", start_time);
    usleep(100000);
    end_time = mf_api_stop_profiling("fcnt1");
    printf("\nEnd: %.9Lf\n", end_time);

    // query database for range of time for all metrics
    /*
    char *res = get_all_data_by_interval("http://mf.excess-project.eu",
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