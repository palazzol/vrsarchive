#	MODIFICATION OF THIS FILE COMITS THE MODIFIER TO FOLLOW THE
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
ALL	= conquer$(EXE)
OPTIONS	= -O -DDEFAULTDIR=\".\" -DLOGIN=\"$(SAFEUID)\"
include $(HOME)/CONFIG

#	This directory is where individual Conquer game data will be stored.
#	Multiple games are now supported.  Each should have its own directory.
DATA = /d7/c7913/smile/oldgame/game.1
DATA = $(GAMLIB)/conquer

OBJ=combat.o commands.o cexecute.o forms.o io.o main.o makeworld.o move.o newlogin.o update.o magic.o npc.o reports.o misc.o randevent.o
FILS=combat.c commands.c cexecute.c forms.c io.c main.c makeworld.c move.c newlogin.c update.c magic.c npc.c reports.c misc.c randevent.c
HEADERS=header.h data.h
SUPT1=nations Makefile help README run man.page
SUPT2=execute messages news

conquer$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

$(OBJ): data.h

$(OBJ): header.h

clobber:
	$(RM) *.o conquer .data core
	$(RM) $(DATA)/conq.* $(GAMDIR)/conquer$(EXE)

clean:
	$(RM) *.o core $(ALL)

$(DATA):
	mkdir -p $@

install: $(GAMDIR) $(DATA) $(ALL)
	echo "#!/bin/sh" >$(GAMDIR)/conquer
	echo $(DATA)/conquer$(EXE) -d$(DATA) >>$(GAMDIR)/conquer
	chmod 755 $(GAMDIR)/conquer
	chown $(SAFEUID) $(DATA)
	chmod 755 $(DATA)
	cp $(ALL) $(DATA)
	cd $(DATA); chown $(SAFEUID) $(ALL)
	cd $(DATA); chmod 4755 $(ALL)
	cp help nations run $(DATA)
	cd $(DATA); chown $(SAFEUID) help nations run
	cd $(DATA); chmod 644 help nations
	cd $(DATA); chmod 700 run
	$(GAMDIR)/conquer -m

lint:
	lint -u $(FILS)

docs:
	sed -e "s/^END//g" help | pr

cpio:
	$(RM) core
	find . -name '*[CrpsEech]' -print | cpio -ocBv > cpiosv

shar:	$(FILS)
	createshar 1 combat.c commands.c forms.c magic.c
	mv shar.out shar1
	createshar 2  move.c makeworld.c main.c
	mv shar.out shar2
	createshar 3 $(SUPT1) header.h update.c
	mv shar.out shar3
	createshar 4 npc.c newlogin.c data.h
	mv shar.out shar4
	createshar 5 cexecute.c reports.c io.c misc.c randevent.c
	mv shar.out shar5

rmshar:
	$(RM) shar1
	$(RM) shar2
	$(RM) shar3
	$(RM) shar4
	$(RM) shar5
