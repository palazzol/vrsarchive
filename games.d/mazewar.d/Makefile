#	12/22/85	@(#)Makefile	1.1
#
CFLAGS	= -O
CC	= cc
SHELL	= /bin/sh

CFILES = main.c talk.c cmd.c view.c init.c scan.c
OFILES = main.o talk.o cmd.o view.o init.o scan.o

war:	${OFILES}
	cd dpy; make dpy.a
	${CC} -o war ${OFILES} dpy/dpy.a -ltermlib

${OFILES}:	war.h

clean:
	rm -f ${OFILES}
