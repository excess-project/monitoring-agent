/**
 * @brief Example showing sending user-defined metrics to the MF server
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mf_api.h>

int
main(int argc, char** argv)
{
    int i, c;
    char* db_key = NULL;
    const char* URL = NULL;
    long double start_time, end_time;

    /***************************************************************************
     * GET OPTIONS
     **************************************************************************/

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

    /***************************************************************************
     * Initialize API and send custom data to the server
     **************************************************************************/

    mf_api_initialize(URL, db_key);

    start_time = mf_api_start_profiling("fcnt1");

    char* mem_info = malloc(256 * sizeof(char));
    for (i = 0; i != 2; ++i) {
        sprintf(mem_info, ",\"mem_used\":%d", i);
        mf_api_send(mem_info);

        usleep(250000);
    }
    free(mem_info);

    end_time = mf_api_stop_profiling("fcnt1");

    usleep(1000000);

    /***************************************************************************
     * Retrieve information from the database
     **************************************************************************/

    char *response = get_data_by_interval(start_time, end_time);
    puts(response);

    response = get_statistics_on_metric_by_interval("mem_used", start_time, end_time);
    puts(response);


    return EXIT_SUCCESS;
}