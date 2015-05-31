CC := gcc
CFLAGS := -I. -L. -std=c99

.PHONY: all
all: libpipara.so piparactl

libpipara.so: pipara.c
	$(CC) -shared -fPIC $(CFLAGS) -o libpipara.so pipara.c

piparactl: piparactl.c libpipara.so
	$(CC) $(CFLAGS) -lpipara -Wall -o piparactl piparactl.c

install: libpipara.so
	cp libpipara.so /usr/local/lib/libpipara.so
	cp pipara.h /usr/local/include/pipara.h
	cp piparactl /usr/local/bin/piparactl
	ldconfig

.PHONY: clean
clean:
	$(RM) *.so
	$(RM) piparactl

