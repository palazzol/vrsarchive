SHELL	= /bin/sh
CFLAGS	= -O

uusnap:	uusnap.c uucp.h
	cc -o $@ $(CFLAGS) uusnap.c

install:
	cp uusnap /usr/local
	chown root /usr/local/uusnap
	chmod 711 /usr/local/uusnap

clean:
	rm -f uusnap.o uusnap
