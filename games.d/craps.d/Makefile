#
#	$Header: /home/Vince/cvs/games.d/craps.d/Makefile,v 1.5 2002-11-23 04:09:37 Vincent Exp $
#
# Use the following defines:
#
#	-DBSD42		for Berkeley Unix 4.2
#	-DBSD29		for Berkeley Unix 2.9
#
ALL	= craps$(EXE)
MODEL	= -i -F 3000
OPTIONS	= -O -DGAMLIB=$(GAMLIB)
include $(HOME)/CONFIG
SOURCES = main.c dont.c etc.c help.c makeb.c pass.c pay.c prints.c \
	  props.c subs.c final.c random.c
OBJECTS = main.o dont.o etc.o help.o makeb.o pass.o pay.o prints.o \
	  props.o subs.o final.o random.o

craps:	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

install: $(ALL) $(GAMDIR) $(GAMLIB) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chown $(SAFEUID) $(ALL)
	cd $(GAMDIR); chmod 4711 $(ALL)
	cd $(GAMLIB); touch craps.list
	cd $(GAMLIB); chown $(SAFEUID) craps.list
	cd $(GAMLIB); chmod 644 craps.list
	cp craps.6 $(MAN6)
	cd $(MAN6); chmod 644 craps.6

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
	rm -f $(ALL) $(OBJECTS)
