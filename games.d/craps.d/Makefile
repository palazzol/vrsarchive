#
# Use the following defines:
#
#	-DBSD42		for Berkeley Unix 4.2
#	-DBSD29		for Berkeley Unix 2.9
#
#CFLAGS  =	-O -DGAMLIB=\"$(GAMLIB) -DBSD42
#CFLAGS  =	-i -O -DGAMLIB=\"$(GAMLIB) -DBSD29
CFLAGS  =	-i -O -DGAMLIB=\"$(GAMLIB) -F 3000
SOURCES =	main.c dont.c etc.c help.c makeb.c pass.c pay.c prints.c \
		props.c subs.c final.c random.c
OBJECTS =	main.o dont.o etc.o help.o makeb.o pass.o pay.o prints.o \
		props.o subs.o final.o random.o
GAMDIR	= $(DSTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib
GAMUSR	= cron
USRMAN	= $(DSTDIR)/usr/man
MANDIR	= $(USRMAN)/man6

craps:	${OBJECTS}
	cc ${CFLAGS} -o craps ${OBJECTS} -lcurses -ltermlib -lm

$(GAMDIR) $(GAMLIB) $(USRMAN) $(MANDIR):
	mkdir $@
	chown $(GAMUSR) $@
	chmod 755 $@

install: craps $(GAMDIR) $(GAMLIB) $(USRMAN) $(MANDIR)
	cp craps $(GAMDIR)
	touch $(GAMLIB)/craps.list
	chown $(GAMUSR) $(GAMDIR)/craps $(GAMLIB)/craps.list
	chmod 4711 $(GAMDIR)/craps
	chmod  644 $(GAMLIB)/craps.list
	cp craps.6 $(MANDIR)
	chmod 644 $(MANDIR)/craps.6

main.o:		main.c types.h
dont.o:		dont.c types.h ext.h
etc.o:		etc.c types.h ext.h
help.o:		help.c types.h ext.h
makeb.o:	makeb.c types.h ext.h
pass.o:		pass.c types.h ext.h
pay.o:		pay.c types.h ext.h
prints.o:	prints.c types.h ext.h
props.o:	props.c types.h ext.h
subs.o:		subs.c types.h ext.h
final.o:	final.c types.h ext.h
random.o:	random.c types.h ext.h

clean:
	rm -f craps *.o
