#
#	Worms %W% %G%
#
SHELL	= /bin/sh
GAMEDIR	= $(DESTDIR)/usr/games
CFLAGS	= -O -i -F 2000
OBJS	= worms.o
CFILES	= worms.c
LIBS	= -lcurses -ltermlib
LFLAGS	= -h

worms: ${OBJS}
	cc $(CFLAGS) ${OBJS} ${LIBS} -o $@

lint: ${CFILES} 
	lint ${LFLAGS} ${CFILES} ${LIBS} 

install: $(GAMEDIR)/worms

$(GAMEDIR)/worms: worms
	cp worms $(GAMEDIR)
	chmod 711 $@

clean:
	rm -f ${OBJS} make.out core worms
