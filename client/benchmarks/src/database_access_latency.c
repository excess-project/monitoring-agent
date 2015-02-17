/**
 * @brief Benchmark determining the latency of retrieving data from Elasticsearch.
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <mf_api.h>

int
main(int argc, char** argv)
{
    int c;
    const char* URL = NULL;
    useconds_t sleep_in_ms = 1000000;

    /***************************************************************************
     * GET OPTIONS
     **************************************************************************/

    opterr = 0;
    while ((c = getopt(argc, argv, "?k:u:t:")) != -1) {
        switch (c) {
            case 't':
                sleep_in_ms = (useconds_t) atoi(optarg);
                break;
            case 'u':
                URL = optarg;
                break;
            case '?':
                if (optopt == 'u') {
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


    struct timespec tim, start, end;
    tim.tv_sec = 0;
    tim.tv_nsec = 5;

    /***************************************************************************
     * Initialize API and send custom data to the server
     **************************************************************************/

    char* id = mf_api_initialize(URL);

    /*
    char* mem_info = malloc(256 * sizeof(char));
    sprintf(mem_info, "\"type\": \"performance\",\"mem_used\":%d", 12345);
    mf_api_send(mem_info);
    free(mem_info);

    int result = usleep(1000000);
    */

    /***************************************************************************
     * Retrieve information from the database
     **************************************************************************/

     /*
    char *response = mf_api_get_data_by_id(id);

    if (result != 0) {
        printf("Error: Could not set usleep to %d", sleep_in_ms);
        return EXIT_FAILURE;
    }

    nanosleep(&tim, NULL);

    response = mf_api_get_data_by_id(id);
    puts(response);
    */

    //nanosleep(&tim, NULL);

    id = "jAGAok4wSD6Htd2E-z5TDQ";

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); // get initial time-stamp

    char* response = mf_api_get_data_by_id(id);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);   // get final time-stamp

    double t_ns = (double)(end.tv_sec - start.tv_sec) * 1.0e9 +
        (double)(end.tv_nsec - start.tv_nsec);

    if (strlen(response) > 10) {
    //puts(response);
        printf("\n\n period: %f", t_ns);
    }
    printf("strlen: %d", strlen(response));

    return EXIT_SUCCESS;
}