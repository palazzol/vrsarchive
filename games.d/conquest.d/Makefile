#	MODIFICATION OF THIS FILE COMMITS THE MODIFIER TO FOLLOW THE
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
MAKE	= /bin/make
CC	= /bin/cc
CFLAGS	= -O 
#GETOPT	= getopt.o	# define this if you don't have it in your library
RM	= /bin/rm -f
LD	= mld
LDFLAGS	= $(LLDFLAGS)
#	This directory is where the executable should be stored
EXECUTABLE = /d7/c7913/smile/newgame
#	This directory is where the data files should be stored
DATA = /d7/c7913/smile/newgame/lib

OBJ=combat.o commands.o execute.o forms.o io.o main.o makeworld.o move.o newlogin.o update.o magic.o npc.o reports.o
FILS=combat.c commands.c execute.c forms.c io.c main.c makeworld.c move.c newlogin.c update.c magic.c npc.c reports.c
HEADERS=header.h
SUPT1=.nations Makefile .help README run
SUPT2=.execute .messages .news

all: $(OBJ) 
	@echo phew... it is time to link, link your hands together and pray this works
	$(CC) $(LDFLAGS) -o $(EXECUTABLE)/conquest $(OBJ) -lcurses -ltermcap
	@echo YAY!  make install if this is your !first! time.  this will set up
	@echo permissions and zero appropriate initial files.  have fun

$(OBJ): header.h 

clobber:
	$(RM) shar1 shar2 shar3 *.o conquest .data core
	$(RM) $(DATA)/conq.* $(EXECUTABLE)/conquest 

clean:
	$(RM) shar1 shar2 shar3 *.o core

install:
	-if test -d $(EXECUTABLE) ; \
	then echo "EXECUTABLE DIRECTORY OK" ; \
	else mkdir $(EXECUTABLE) ; \
	fi 
	-if test -d $(DATA) ; \
	then echo "DATA DIRECTORY OK" ; \
	else mkdir $(DATA) ; \
	fi 
	cp .help $(DATA)/.help
	cp .nations $(DATA)/.nations
	cp run $(DATA)/run
	> $(DATA)/.execute
	> $(DATA)/.messages
	> $(DATA)/.news
	chmod 0600 *
	chmod 0777 $(EXECUTABLE) $(DATA) 
	chmod 0700 $(DATA)/run
	chmod 0666 $(DATA)/.execute 
	chmod 0644 $(DATA)/.help $(DATA)/.news $(DATA)/.messages
	chmod 0777 $(EXECUTABLE)/conquest

docs:
	sed -e "s/^END//g" .help | pr
