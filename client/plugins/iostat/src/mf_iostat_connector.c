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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "mf_iostat_connector.h"

int SUCCESS = 0;
char IOSTAT_MODE[2];
int IS_INITALIZED = 0;
const char* EXECUTABLE = "/usr/bin/iostat";

static int
check_iostat()
{
    if (IS_INITALIZED) {
        return SUCCESS;
    }

    struct stat sb;
    if (stat(EXECUTABLE, &sb) == 0 && sb.st_mode & S_IXUSR) {
        SUCCESS = 1;
    }

    IS_INITALIZED = 1;

    return SUCCESS;
}

void
mf_iostat_init(char** named_events, size_t num_events)
{
    int i;
    int DISK_MODE = 0;
    int CPU_MODE = 0;

    for (i = 0; i != num_events; ++i) {
        if (strcmp(named_events[i], "DISK") == 0) {
           DISK_MODE = 1;
        }
        if (strcmp(named_events[i], "CPU") == 0) {
           CPU_MODE = 1;
        }
    }

    if (DISK_MODE && CPU_MODE) {
        strcpy(IOSTAT_MODE, "");
    } else if (DISK_MODE) {
        strcpy(IOSTAT_MODE, "-d");
    } else if (CPU_MODE) {
        strcpy(IOSTAT_MODE, "-c");
    } else {
        strcpy(IOSTAT_MODE, "");
    }

    check_iostat();
}

int starts_with(const char* a, const char* b)
{
   return (strncmp(a, b, strlen(b)) == 0) ? 1 : 0;
}

static void
tokenize_events(char* line, Iostat_Plugin *iostat)
{
    char* snippet;
    char* token = strtok(line, " ");
    token = strtok(NULL, " ");
    while (token != NULL) {
        iostat->events[iostat->num_events] = malloc(sizeof(char) * 32);
        if ((snippet = strchr(token, '\n')) != NULL) {
            *snippet = '\0';
        }
        char* name = malloc(sizeof(char) * 32);
        sprintf(name, "iostat:avg-cpu:%s", token);
        strcpy(iostat->events[iostat->num_events], name);
        iostat->num_events++;
        token = strtok(NULL, " ");
    }
}

static void
tokenize_values(char* line, Iostat_Plugin *iostat)
{
    int i = 0;
    char* snippet;
    char* token = strtok(line, " ");
    while (token != NULL) {
        iostat->values[i] = malloc(sizeof(char) * 32);
        if ((snippet = strchr(token, '\n')) != NULL) {
            *snippet = '\0';
        }
        strcpy(iostat->values[i], token);
        token = strtok(NULL, " ");
        i++;
    }
}

static void
tokenize_disk_mode(char* command, Iostat_Plugin *iostat)
{
    FILE *fp;
    char path[1035];

    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    int i = 0;
    char *units[16];
    char *snippet;

    while (fgets(path, sizeof(path), fp) != NULL) {
        if (starts_with(path, "Device")) {
            char* token = strtok(path, " ");
            token = strtok(NULL, " ");
            while (token != NULL) {
                units[i] = malloc(sizeof(char) * 16);
                if ((snippet = strchr(token, '\n')) != NULL) {
                    *snippet = '\0';
                }
                strcpy(units[i], token);
                token = strtok(NULL, " ");
                i++;
            }


            while (fgets(path, sizeof(path), fp) != NULL) {
                i = 0;
                char* token = strtok(path, " ");
                char* device = malloc(sizeof(char) * 32);
                while (token != NULL) {
                    if (i == 0) {
                        strcpy(device, token);
                        token = strtok(NULL, " ");
                        i++;
                        continue;
                    }
                    iostat->events[iostat->num_events] = malloc(sizeof(char) * 32);
                    iostat->values[iostat->num_events] = malloc(sizeof(char) * 32);
                    char *name = malloc(sizeof(char) * 32);
                    sprintf(name, "iostat:%s:%s", device, units[i-1]);
                    strcpy(iostat->events[iostat->num_events], name);
                    if ((snippet = strchr(token, '\n')) != NULL) {
                       *snippet = '\0';
                    }
                    strcpy(iostat->values[iostat->num_events], token);
                    token = strtok(NULL, " ");
                    i++;
                    iostat->num_events++;
                }
            }
        }
    }

    pclose(fp);
}

static void
tokenize_all(char* command, Iostat_Plugin *iostat)
{
    FILE *fp;
    char path[1035];

    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    int i = 0;
    char *units[16];
    char *snippet;

    while (fgets(path, sizeof(path), fp) != NULL) {
        if (starts_with(path, "avg-cpu")) {
            tokenize_events(path, iostat);
            if (fgets(path, sizeof(path), fp) != NULL) {
                tokenize_values(path, iostat);
            }
        }
        if (starts_with(path, "Device")) {
            char* token = strtok(path, " ");
            token = strtok(NULL, " ");
            while (token != NULL) {
                units[i] = malloc(sizeof(char) * 16);
                if ((snippet = strchr(token, '\n')) != NULL) {
                    *snippet = '\0';
                }
                strcpy(units[i], token);
                token = strtok(NULL, " ");
                i++;
            }


            while (fgets(path, sizeof(path), fp) != NULL) {
                i = 0;
                char* token = strtok(path, " ");
                char* device = malloc(sizeof(char) * 32);
                while (token != NULL) {
                    if (i == 0) {
                        strcpy(device, token);
                        token = strtok(NULL, " ");
                        i++;
                        continue;
                    }
                    iostat->events[iostat->num_events] = malloc(sizeof(char) * 32);
                    iostat->values[iostat->num_events] = malloc(sizeof(char) * 32);
                    char *name = malloc(sizeof(char) * 32);
                    sprintf(name, "iostat:%s:%s", device, units[i-1]);
                    strcpy(iostat->events[iostat->num_events], name);
                    if ((snippet = strchr(token, '\n')) != NULL) {
                       *snippet = '\0';
                    }
                    strcpy(iostat->values[iostat->num_events], token);
                    token = strtok(NULL, " ");
                    i++;
                    iostat->num_events++;
                }
            }
        }
    }

    pclose(fp);
}

void
mf_iostat_read(Iostat_Plugin *iostat)
{
    if (!check_iostat()) {
        return;
    }

    if (iostat == NULL) {
        iostat = malloc(sizeof(Iostat_Plugin));
    }
    memset(iostat, 0, sizeof(Iostat_Plugin));
    iostat->num_events = 0;

    char command[32];
    sprintf(command, "%s %s", EXECUTABLE, IOSTAT_MODE);

    FILE *fp;
    char path[1035];

    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    if (strcmp(IOSTAT_MODE, "-c") == 0) {
        while (fgets(path, sizeof(path), fp) != NULL) {
            if (starts_with(path, "avg-cpu")) {
                tokenize_events(path, iostat);
                if (fgets(path, sizeof(path), fp) != NULL) {
                    tokenize_values(path, iostat);
                }
            }
        }
    } else if (strcmp(IOSTAT_MODE, "-d") == 0) {
        tokenize_disk_mode(command, iostat);
    } else {
        tokenize_all(command, iostat);
    }

    pclose(fp);
}