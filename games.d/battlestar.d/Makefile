#
#	Battelstar %W% %G%
#
SHELL	= /bin/sh
CFLAGS	= -O -Ml
GAMEDIR	= $(DESTDIR)/usr/games
GAMELIB	= $(GAMEDIR)/lib
USRMAN	= $(DESTDIR)/usr/man
MANDIR	= $(USRMAN)/man6
LOGFILE	= $(GAMELIB)/battlestar.log
DAYF	= $(GAMELIB)/dayfile
NIGHTF	= $(GAMELIB)/nightfile
MAN	= $(MANDIR)/battlestar.6
OBJS	= battlestar.o com1.o com2.o com3.o com4.o com5.o com6.o \
	  com7.o init.o cypher.o getcom.o parse.o room.o save.o fly.o \
	  misc.o globals.o wlist.o dayobjs.o nightobjs.o initrms.o
CFILES	= battlestar.c com1.c com2.c com3.c com4.c com5.c com6.c \
	  com7.c init.c cypher.c getcom.c parse.c room.c save.c fly.c \
	  misc.c globals.c initrms.c
LIBS	= -lcurses -ltermlib
LFLAGS	= -h
JUNKF	= battlestar tags mkfile mkwlist mkobjs \
	  wlist.c dayobjs.c nightobjs.c mkwlist.o mkobjs.o
DATAF	= dayfile nightfile battlewords dayobjs nightobjs

battlestar: ${OBJS}
	cc $(CFLAGS) ${OBJS} ${LIBS} -o battlestar

$(DATAF): ;

mkfile: mkfile.c
	$(CC) $(CFLAGS) -o mkfile mkfile.c

mkwlist: mkwlist.c
	$(CC) $(CFLAGS) -o mkwlist mkwlist.c

wlist.c: battlewords mkwlist
	mkwlist battlewords

mkobjs: mkobjs.c
	$(CC) $(CFLAGS) -o mkobjs mkobjs.c

dayobjs.c: dayobjs mkobjs
	mkobjs dayobjs

nightobjs.c: nightobjs mkobjs
	mkobjs nightobjs

lint: ${CFILES} 
	lint ${LFLAGS} ${CFILES} ${LIBS} 

$(GAMEDIR) $(GAMELIB) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: $(GAMEDIR)/battlestar $(USRMAN) $(MAN) $(LOGFILE) $(DAYF) $(NIGHTF)

$(GAMEDIR)/battlestar: $(GAMEDIR) battlestar
	cp battlestar $(GAMEDIR)
	chown cron $@
	chgrp cron $@
	chmod 4711 $@

$(DAYF): $(GAMELIB) dayfile
	cp dayfile $@
	chown cron $@
	chgrp cron $@
	chmod 600 $@

$(NIGHTF): $(GAMELIB) nightfile
	cp nightfile $@
	chown cron $@
	chgrp cron $@
	chmod 600 $@

$(LOGFILE): $(GAMELIB)
	touch $(LOGFILE)
	chown cron $@
	chgrp cron $@
	chmod 644 $(LOGFILE)

$(MAN): $(MANDIR) battlestar.6
	cp battlestar.6 $@
	chmod 644 $@

clean:
	rm -f ${OBJS} ${JUNKF}
