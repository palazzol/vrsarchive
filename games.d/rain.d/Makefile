#
#	Rain %W% %G%
#
SHELL	= /bin/sh
GAMEDIR	= $(DESTDIR)/usr/games
CFLAGS	= -O -i -F 2000
OBJS	= rain.o
CFILES	= rain.c
LIBS	= -lcurses -ltermlib
LFLAGS	= -h

rain: ${OBJS}
	cc $(CFLAGS) ${OBJS} ${LIBS} -o $@

lint: ${CFILES} 
	lint ${LFLAGS} ${CFILES} ${LIBS} 

install: $(GAMEDIR)/rain

$(GAMEDIR)/rain: rain
	cp rain $(GAMEDIR)
	chmod 711 $@

clean:
	rm -f ${OBJS} make.out core rain
