#
#	vtrek @(#)Makefile	1.4 10/3/86
#
OBJS	= main.o plot.o sub1.o sub2.o termio.o
OPTIONS	= -O -DVTREKINS=\"$(GAMLIB)/vtrek.doc\"
MODEL	= -i -F 2000
ALL	= vtrek$(EXE)
include $(HOME)/CONFIG

vtrek:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

$(OBJS): vtrek.h

install: $(GAMDIR) $(GAMLIB) $(ALL)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp vtrek.doc $(GAMLIB)
	chmod 644 $(GAMLIB)/vtrek.doc

clean:
	rm -f $(OBJS) $(ALL) make.out core
