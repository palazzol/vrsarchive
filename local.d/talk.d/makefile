#
#	@(#)Makefile	1.3 12/3/84
#
SHELL	= /bin/sh
OBJECT	= talk.o announce.o ctl.o display.o get_names.o init_disp.o invite.o \
	  io.o msgs.o
SOURCE	= talk.c announce.c ctl.c display.c get_names.c init_disp.c invite.c \
	  io.c msgs.c
INCLUDE	= talk.h
CFLAGS	= -O
INSTALL	= /usr/local

talk:	${OBJECT}
	cc -F 2000 -o talk ${OBJECT} -lcurses -ltermlib -lx

install: talk
	cp talk $(INSTALL)
	chmod 711 $(INSTALL)/talk

${OBJECT}: $(INCLUDE)

lint:
	lint -lcurses -ltermlib ${SOURCE} | tee lint.out

clean:
	rm -f *.o lint.out tags talk

tags:	${SOURCE} ${INCLUDE}
	rm -f tags
	ctags ${SOURCE} ${INCLUDE}
