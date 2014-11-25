CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(PAPI_INC) $(CURL_INC)
LFLAGS = -lm $(CURL) $(PAPI)

## External Libraries
CURL = -L../../binaries/curl/lib/ -lcurl
CURL_INC = -I../../binaries/curl/include/

CUTEST = ../libs/CuTest

## Plugins
LIKWID_CFLAGS = $(CFLAGS) $(LIKWID_INC) -lpthread
LIKWID_LFLAGS = $(LFLAGS) $(LIKWID)

PAPI = -L../../binaries/papi/lib/ -lpapi
PAPI_INC =  -I../../binaries/papi/include/

LIKWID = -L../../binaries/likwid/lib/ -llikwid
LIKWID_INC = -I../../binaries/likwid/include/likwid

## Debug
DEBUG ?= 1
ifeq (${DEBUG}, 1)
    CFLAGS += -DDEBUG -g
else
    CFLAGS += -DNDEBUG -g
endif

## Targets
all: test_papi test_rapl test_likwid

test_papi:
	$(CC) -o test_papi $(CUTEST)/*.c test_papi.c ../plugins/papi_plugin.c $(CFLAGS) $(LFLAGS)

test_rapl:
	$(CC) -o test_rapl $(CUTEST)/*.c test_rapl.c ../plugins/rapl_plugin.c $(CFLAGS) $(LFLAGS)

test_likwid:
	$(CC) -o test_likwid $(CUTEST)/*.c test_likwid.c ../plugins/likwid_plugin.c $(LIKWID_CFLAGS) $(LIKWID_LFLAGS)

clean:
	rm -rf test_papi
	rm -rf test_rapl
	rm -rf test_likwid
