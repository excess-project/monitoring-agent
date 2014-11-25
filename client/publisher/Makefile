CC = /usr/bin/gcc 

COPT_SO = $(CFLAGS) -fpic

CFLAGS = -std=gnu99 -pedantic -Wall -Wwrite-strings -Wpointer-arith \
-Wcast-align -O0 -ggdb $(CURL_INC)

LFLAGS =  -lm $(CURL)

CURL = -L../../binaries/curl/lib/ -lcurl
CURL_INC = -I../../binaries/curl/include/

all: clean-all libpublisher.a

publisher.o:
		$(CC) -c src/publisher.c -lrt -ldl -Wl,--export-dynamic $(CFLAGS) $(LFLAGS)

libpublisher.a:	publisher.o
		ar rcs $@ $^

clean:
	rm -rf *.o

clean-all:
	rm -rf *.o *.a

doc: $(FILES)
	doxygen Doxyfile

