CC = /usr/bin/gcc 

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -fPIC -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(PAPI_INC) $(DEBUG_INC)

LFLAGS = $(PAPI)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
endif

COMMON = ../..
SRC = src
TESTS = tests
UTILS = $(SRC)/utils
PAPI = -L$(COMMON)/../binaries/papi/lib -lpapi
PAPI_INC = -I$(COMMON)/../binaries/papi/include/
DEBUG_INC = -I$(COMMON)/

all: clean mf_papi_client

mf_papi_client:
	$(CC) $(UTILS)/mf_papi_client.c -o mf_papi_client $(CFLAGS) $(LFLAGS)

clean:
	rm -rf *.o
	rm -f mf_papi_client

doc: $(FILES)
	doxygen Doxyfile