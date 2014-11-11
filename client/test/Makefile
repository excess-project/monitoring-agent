CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall
LIKWID_CFLAGS = $(CFLAGS) -lm -lpthread

PAPI = ../../binaries/papi/lib/libpapi.a
PAPI_INC =  -I../../binaries/papi/include/

LIKWID = ../../likwid-3.1/liblikwid.a
LIKWID_INC = -I../../likwid-3.1/src/includes -I../../likwid-3.1/GCC

all: test_parser test_papi test_rapl test_likwid

test_parser:
	$(CC) -o test_parser AllTests.c CuTest.c test_parser.c ../plugins/parser.c $(PAPI) $(PAPI_INC) $(CFLAGS)

test_papi:
	$(CC) -o test_papi AllTests.c CuTest.c test_papi.c ../plugins/parser.c ../plugins/papi_plugin.c $(PAPI) $(PAPI_INC) $(CFLAGS)

test_rapl:
	$(CC) -o test_rapl AllTests.c CuTest.c test_rapl.c ../plugins/parser.c ../plugins/rapl_plugin.c $(PAPI) $(PAPI_INC) $(CFLAGS)

test_likwid:
	$(CC) -o test_likwid AllTests.c CuTest.c test_likwid.c ../plugins/likwid_plugin.c $(LIKWID) $(LIKWID_INC) $(LIKWID_CFLAGS)
	
clean:
	rm -rf test_parser
	rm -rf test_papi
	rm -rf test_rapl
	rm -rf test_likwid