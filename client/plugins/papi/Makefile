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
PAPI = -L$(COMMON)/../binaries/papi/lib -lpapi
PAPI_INC = -I$(COMMON)/../binaries/papi/include/
DEBUG_INC = -I$(COMMON)/

all: clean mf_papi

mf_papi:
	$(CC) $(SRC)/mf_papi.c -o mf_papi $(CFLAGS) $(LFLAGS)

clean:
	rm -rf *.o
	rm -f mf_papi

doc: $(FILES)
	doxygen Doxyfile