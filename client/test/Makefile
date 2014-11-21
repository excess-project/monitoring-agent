CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall
LIKWID_CFLAGS = $(CFLAGS) -lm -lpthread

PAPI = ../../binaries/papi/lib/libpapi.a
PAPI_INC =  -I../../binaries/papi/include/

LIKWID = ../../binaries/likwid/lib/liblikwid.a
LIKWID_INC = -I../../binaries/likwid/include/likwid

CURL = -L../../binaries/curl/lib/ -lcurl
CURL_INC = -I../../binaries/curl/include/

LFLAGS =  -lm $(CURL)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
endif

all: test_parser test_papi test_rapl test_likwid test_publisher test_ini_parser

test_parser:
	$(CC) -o test_parser AllTests.c CuTest.c test_parser.c ../plugins/parser.c $(PAPI) $(PAPI_INC) $(CFLAGS)

test_papi:
	$(CC) -o test_papi AllTests.c CuTest.c test_papi.c ../plugins/parser.c ../plugins/papi_plugin.c $(PAPI) $(PAPI_INC) $(CFLAGS)

test_rapl:
	$(CC) -o test_rapl AllTests.c CuTest.c test_rapl.c ../plugins/parser.c ../plugins/rapl_plugin.c $(PAPI) $(PAPI_INC) $(CFLAGS)

test_likwid:
	$(CC) -o test_likwid AllTests.c CuTest.c test_likwid.c ../plugins/likwid_plugin.c $(LIKWID) $(LIKWID_INC) $(LIKWID_CFLAGS)

test_publisher:
	$(CC) -o test_publisher AllTests.c CuTest.c test_publisher.c ../publisher.c $(CFLAGS) $(CURL_INC) $(LFLAGS)
	
test_ini_parser:
	$(CC) -o test_ini_parser AllTests.c CuTest.c test_ini_parser.c ../ini_parser.c ../ini.c $(CFLAGS)

clean:
	rm -rf test_parser
	rm -rf test_papi
	rm -rf test_rapl
	rm -rf test_likwid
	rm -rf test_publisher
	rm -rf test_ini_parser
