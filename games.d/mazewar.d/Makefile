#
#	$Header: /home/Vince/cvs/games.d/mazewar.d/Makefile,v 1.6 2002-11-23 04:09:41 Vincent Exp $
#
ALL	= war$(EXE)
MODEL	= -M2l -F 3000
OPTIONS	= -DLIBDIR=\"$(GAMLIB)\" -DOBJECTFILE=\"$(GAMLIB)/objects.war\" \
	  -I$(INCDIR) -O
include $(HOME)/CONFIG
MYLIBS	= $(DPYLIB) $(BERKLIB) $(LIBS)
CFILES	= main.c talk.c cmd.c view.c init.c scan.c
OFILES	= main.o talk.o cmd.o view.o init.o scan.o

war$(EXE):	$(OFILES)
		$(CC) $(CFLAGS) -o $@ $(OFILES) $(MYLIBS)

$(OFILES):	war.h

install:	$(ALL) $(GAMDIR) $(GAMLIB) $(MAN6)
		cp $(ALL) $(GAMDIR)
		cd $(GAMDIR); chmod 711 $(ALL)
		cp objects.war boards.war $(GAMLIB)
		cd $(GAMLIB); chmod 644 objects.war boards.war
		cp war.6 $(MAN6)
		chmod 644 $(MAN6)/war.6

clean:
		rm -f $(OFILES) $(ALL)
