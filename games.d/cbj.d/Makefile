#
#	Blackjack %W% %G%
#
GAMEDIR	= /usr/games
USRMAN	= /usr/man
MANDIR	= $(USRMAN)/man6
#
SHELL	= /bin/sh
CFILES	= cbj.c
OBJS	= cbj.o
CFLAGS	= -O

cbj:	${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o cbj

$(GAMEDIR) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: cbj $(GAMEDIR) $(USRMAN) $(MANDIR)
	cp cbj $(GAMEDIR)/cbj
	chmod 711 $(GAMEDIR)/cbj
	cp cbj.6 $(MANDIR)
	chmod 644 $(MANDIR)/cbj.6

clean:
	rm -f ${OBJS} cbj
