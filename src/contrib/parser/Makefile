CC = gcc

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb -fPIC $(APR_INC) $(CORE_INC)

LFLAGS =  -lm $(APR)

COMMON = ../..
CORE_INC = -I$(COMMON)/core
BINARIES = ../../../bin
APR_CONFIG = $(BINARIES)/apr/bin/apr-1-config
APU_CONFIG = $(BINARIES)/apr/bin/apu-1-config
APR = $(shell $(APR_CONFIG) --link-ld) $(shell $(APU_CONFIG) --link-ld)
APR_INC = $(shell $(APR_CONFIG) --includes) $(shell $(APR_CONFIG) --includes)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -g
else
	CFLAGS += -DNDEBUG
endif

all: clean-all libparser.so libparser.a

mf_parser.o:
	$(CC) -c src/mf_parser.c libs/ini/ini.c -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

ini.o:
	$(CC) -c libs/ini/ini.c -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

libparser.so: mf_parser.o ini.o
	$(CC) -shared -o $@ $^ -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

libparser.a: mf_parser.o ini.o
	ar rcs $@ $^

clean:
	rm -rf *.o

clean-all:
	rm -rf *.o *.a *.so

doc: $(FILES)
	doxygen Doxyfile
