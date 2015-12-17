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

#include "excess_main.h"
#include "util.h"

int foo(int bar) {
    return 1;
}

int getFQDN(char *fqdn) {
    struct addrinfo hints, *info, *p;

    int gai_result;

    char *hostname = malloc(sizeof(char) * 80);
    gethostname(hostname, sizeof hostname);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 1) {
        //fprintf(stderr, "getaddrinfo: %s,\n using regular hostname\n",
        //        gai_strerror(gai_result));
        FILE *tmp = NULL;
        if ((tmp = popen("hostname", "r")) == NULL ) {
            perror("popen");
            return -1;
        }
        char line[200];
        while (fgets(line, 200, tmp) != NULL )
            sprintf(fqdn, "%s", line);
        return 1;
    }
    for (p = info; p != NULL ; p = p->ai_next) {
        sprintf(fqdn, "%s\n", p->ai_canonname);
    }

    if (info->ai_canonname)
        freeaddrinfo(info);

    return 1;
}