#
#	noughts %W% %G%
#
SHELL	= /bin/sh
SOURCES	= adm.c automat.c autom.c chose.c inmove.c\
	  terminal.c upd.c catch.c  updtht.c utboard.c save.c
OBJECTS	= adm.o automat.o autom.o chose.o inmove.o\
	  terminal.o upd.o catch.o  updtht.o utboard.o save.o
CFLAGS	= -F 2000
GAMDIR	= $(DESTDIR)/usr/games

noughts: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -ltermcap

clean:
	rm -f $(OBJECTS) noughts core

$(GAMDIR):
	mkdir $@
	chmod 755 $@

install: $(GAMDIR) noughts
	cp noughts $(GAMDIR)/noughts
	chmod 711 $(GAMDIR)/noughts

print:
	print makefile def.h adm.c automat.c autom.c chose.c inmove.c
	print terminal.c upd.c catch.c  updtht.c utboard.c save.c

adm.o autom.o chose.o inmove.o terminal.o: def.h
upd.o catch.o  updtht.o utboard.o save.o: def.h
