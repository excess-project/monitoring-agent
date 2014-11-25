CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall $(PAPI_INC) $(CURL_INC)
LFLAGS = -lm $(CURL) $(PAPI)

LIKWID_CFLAGS = $(CFLAGS) -lm -lpthread

PAPI = ../../binaries/papi/lib/libpapi.a
PAPI_INC =  -I../../binaries/papi/include/

LIKWID = ../../binaries/likwid/lib/liblikwid.a
LIKWID_INC = -I../../binaries/likwid/include/likwid

CURL = -L../../binaries/curl/lib/ -lcurl
CURL_INC = -I../../binaries/curl/include/

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
endif

all: test_papi test_rapl test_likwid test_publisher

test_papi:
	$(CC) -o test_papi AllTests.c CuTest.c test_papi.c ../plugins/papi_plugin.c $(CFLAGS) $(LFLAGS)

test_rapl:
	$(CC) -o test_rapl AllTests.c CuTest.c test_rapl.c ../plugins/rapl_plugin.c $(CFLAGS) $(LFLAGS)

test_likwid:
	$(CC) -o test_likwid AllTests.c CuTest.c test_likwid.c ../plugins/likwid_plugin.c $(LIKWID) $(LIKWID_INC) $(LIKWID_CFLAGS) $(LFLAGS)

test_publisher:
	$(CC) -o test_publisher AllTests.c CuTest.c test_publisher.c ../publisher.c $(CFLAGS) $(LFLAGS)

clean:
	rm -rf test_papi
	rm -rf test_rapl
	rm -rf test_likwid
	rm -rf test_publisher