#	MODIFICATION OF THIS FILE COMMITS THE MODIFIER TO FOLLOW THE
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
OPTIONS	= -DDATA=$(DATA)
MODEL	= -M2l
ALL	= conquest
include /etc/CONFIG
#GETOPT	= getopt.o	# define this if you don't have it in your library
RM	= /bin/rm -f
DATA	= $(GAMLIB)/conquest

OBJ=combat.o commands.o execute.o forms.o io.o main.o makeworld.o move.o newlogin.o update.o magic.o npc.o reports.o
FILS=combat.c commands.c execute.c forms.c io.c main.c makeworld.c move.c newlogin.c update.c magic.c npc.c reports.c
HEADERS=header.h
SUPT1=.nations Makefile .help README run
SUPT2=.execute .messages .news

conquest: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lcurses -ltermcap

lint:	$(OBJ:.o=.c)
	lint $(OPTIONS) -lcurses -ltermcap $(OBJ:.o=.c) >$@

$(OBJ): header.h

clean:
	$(RM) shar1 shar2 shar3 *.o core $(ALL)

$(DATA):
	mkdir $@
	chmod 777 $@

install: $(ALL) $(GAMDIR) $(GAMLIB) $(DATA)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 0711 $(ALL)
	cp .help $(DATA)/.help
	cp .nations $(DATA)/.nations
	cp run.sh $(DATA)/run
	cd $(DATA); touch $(DATA)/.execute $(DATA)/.messages $(DATA)/.news
	cd $(DATA); chmod 0600 *
	chmod 0755 $(DATA)/run
	chmod 0666 $(DATA)/.execute
	chmod 0644 $(DATA)/.help $(DATA)/.news $(DATA)/.messages

docs:
	sed -e "s/^END//g" .help | pr
