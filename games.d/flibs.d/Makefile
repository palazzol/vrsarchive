#
#	flibs $Header: /home/Vince/cvs/games.d/flibs.d/Makefile,v 1.4 2002-11-23 04:09:38 Vincent Exp $
#
ALL	= flibs$(EXE)
MODEL	= -i
OPTIONS	= -O
OBJS	= flibs.o fopsys.o fterminal.o
include $(HOME)/CONFIG

flibs:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

install: $(ALL) $(GAMDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp flibs.6 $(MAN6)
	chmod 644 $(MAN6)/flibs.6

clean:
	rm -f $(OBJS) $(ALL)
