#
#	vtrek @(#)Makefile	1.2 9/20/85
#
SHELL	= /bin/sh
OBJS	= main.o plot.o sub1.o sub2.o termio.o
LIBS	= -lm -ltermcap
GAMDIR	= $(DESTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib
CFLAGS	= -O -DVTREKINS=\"$(GAMLIB)/vtrek.doc\"

vtrek:	$(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

$(OBJS): vtrek.h

$(GAMDIR) $(GAMLIB):
	mkdir $@
	chmod 755 $@

install: $(GAMDIR) $(GAMLIB) vtrek
	cp vtrek $(GAMDIR)
	chmod 711 $(GAMDIR)/vtrek
	cp vtrek.doc $(GAMLIB)
	chmod 644 $(GAMLIB)/vtrek.doc

clean:
	rm -f $(OBJS) vtrek make.out core
