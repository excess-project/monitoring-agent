#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mf_api.h"

int
main(int argc, char** argv)
{
    int i, c;
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

    char* mem_info = malloc(256 * sizeof(char));
    for (i = 0; i != 2; ++i) {
        sprintf(mem_info, ",\"mem_used\":%d", i);
        mf_api_send(mem_info);

        usleep(500000);

        sprintf(mem_info, ",\"mem_not_used\":%d", i);
        mf_api_send(mem_info);
    }
    free(mem_info);

    end_time = mf_api_stop_profiling("fcnt1");

    usleep(1000000);

    char *response = get_data_by_interval(start_time, end_time);
    puts(response);

    response = get_data_by_metric_by_interval("mem_used", start_time, end_time);
    puts(response);

    return EXIT_SUCCESS;
}