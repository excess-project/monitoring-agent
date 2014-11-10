CC = /usr/bin/gcc

CFLAGS = -std=gnu99 -pedantic -Wall

PAPI = ../../binaries/papi/lib/libpapi.a
PAPI_INC =  -I../../binaries/papi/include/

all: test_parser test_papi test_rapl

test_parser:
	$(CC) -o test_parser AllTests.c CuTest.c test_parser.c ../plugins/parser.c $(PAPI) $(PAPI_INC) $(CFLAGS)

test_papi:
	$(CC) -o test_papi AllTests.c CuTest.c test_papi.c ../plugins/parser.c ../plugins/papi_plugin.c $(PAPI) $(PAPI_INC) $(CFLAGS)

test_rapl:
	$(CC) -o test_rapl AllTests.c CuTest.c test_rapl.c ../plugins/parser.c ../plugins/rapl_plugin.c $(PAPI) $(PAPI_INC) $(CFLAGS)
	
clean:
	rm -rf test_parser
	rm -rf test_papi
	rm -rf test_rapl