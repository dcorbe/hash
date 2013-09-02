CC=gcc
CFLAGS=-ggdb
LDFLAGS=-ggdb

hash: hash.o

clean:
	rm -rf hash *.o *.core
