#
#	noughts $Header: /home/Vince/cvs/games.d/noughts.d/Makefile,v 1.2 1987-06-08 21:27:07 vrs Exp $
#
ALL	= noughts
MODEL	= -i -F 2000
OPTIONS	= -O
include ../../CONFIG
SOURCES	= adm.c automat.c autom.c chose.c inmove.c\
	  terminal.c upd.c catch.c  updtht.c utboard.c save.c
OBJECTS	= adm.o automat.o autom.o chose.o inmove.o\
	  terminal.o upd.o catch.o  updtht.o utboard.o save.o

noughts: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

print:
	print Makefile def.h adm.c automat.c autom.c chose.c inmove.c
	print terminal.c upd.c catch.c  updtht.c utboard.c save.c

adm.o autom.o chose.o inmove.o terminal.o: def.h
upd.o catch.o updtht.o save.o utboard.o: def.h
