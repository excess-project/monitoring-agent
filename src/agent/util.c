/*
 * Copyright 2014, 2015 High Performance Computing Center, Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

#include "excess_main.h"
#include "mf_types.h"
#include "util.h"

int getFQDN(char *fqdn) {
    struct addrinfo hints, *info, *p;

    int gai_result;

    char hostname[1024];
    gethostname(hostname, 1024);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 1) {
        FILE *tmp = NULL;
        if ((tmp = popen("hostname", "r")) == NULL ) {
            perror("popen");
            return -1;
        }
        char line[200];
        while (fgets(line, 200, tmp) != NULL ) {
            sprintf(fqdn, "%s", line);
        }
        fclose(tmp);

        return 1;
    }
    for (p = info; p != NULL ; p = p->ai_next) {
        sprintf(fqdn, "%s\n", p->ai_canonname);
    }

    if (info->ai_canonname) {
        freeaddrinfo(info);
    }

    free(info);
    free(p);

    return 1;
}

void
convert_time_to_char(long double ts, char* time_stamp)
{
    time_t second = (time_t) floorl(ts);
    time_t usec = (time_t)((ts - second) * 1e6);
    /* get timestamp */
    char fmt[64], buf[64];
    struct tm *tm;
    if((tm = localtime(&second)) != NULL) {
        // yyyy-MM-dd’T'HH:mm:ss.SSS
        strftime(fmt, sizeof fmt, "%Y-%m-%dT%H:%M:%S.%%6u", tm);
        snprintf(buf, sizeof buf, fmt, usec);
    }

    memcpy(time_stamp, buf, strlen(buf) - 3);
    time_stamp[strlen(buf) - 3] = '\0';

    /* replace whitespaces in timestamp: yyyy-MM-dd’T'HH:mm:ss. SS */
    int i = 0;
    while (time_stamp[i]) {
        if (isspace(time_stamp[i])) {
            time_stamp[i] = '0';
        }
        i++;
    }
}

/*free the string contained in a metric*/
void free_metric (metric a_metric) {
    free(a_metric->msg);
    a_metric->msg = (char *) 0;
}

/*free a bulk of metrics*/
void free_bulk (metric *resMetrics, int size) {
    int i;
    for (i=0; i<size; i++) {
        if(resMetrics[i] != NULL) {
            free_metric(resMetrics[i]);
            free(resMetrics[i]);    
        }
    }
    free(resMetrics);
}