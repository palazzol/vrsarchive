#
# makefile: Rog-O-Matic XIV (CMU) Thu Jan 31 18:23:25 1985 - mlm
# Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
#
ALL	= rogomatic player rgmplot datesub histplot gene
PUBDIR	= $(GAMLIB)/rgm14
OPTIONS	= -O -DGAMDIR=\"$(GAMDIR) -DGAMLIB=\"$(PUBDIR)
MODEL	= -M2l -F 2000
include ../../CONFIG
OBJS	= arms.o command.o database.o debug.o explore.o io.o learn.o\
	  ltm.o main.o mess.o monsters.o pack.o rand.o replay.o rooms.o\
	  scorefile.o search.o stats.o strategy.o survival.o tactics.o\
	  things.o titlepage.o utility.o worth.o 
SRCS	= arms.c command.c database.c debug.c explore.c io.c learn.c\
	  ltm.c main.c mess.c monsters.c pack.c rand.c replay.c rooms.c\
	  scorefile.c search.c stats.c strategy.c survival.c tactics.c\
	  things.c titlepage.c utility.c worth.c 
HDRS	= types.h globals.h install.h termtokens.h
OTHERS	= setup.c findscore.c datesub.l histplot.c rgmplot.c gene.c\
	  rplot Bugreport
#
# General makefile stuff:
#
anim: anim.c utility.o
	$(CC) -o $@ anim.c utility.o $(LIBS)
rgmplot: rgmplot.o utility.o
	$(CC) $(CFLAGS) -o $@ rgmplot.o utility.o $(LIBS)
rogomatic: setup.o findscore.o scorefile.o utility.o
	$(CC) $(CFLAGS) -o $@ setup.o findscore.o scorefile.o utility.o $(LIBS)
datesub.c: datesub.l
	lex datesub.l
	mv lex.yy.c datesub.c
gene: gene.c rand.o learn.o stats.o utility.o types.h install.h
	$(CC) $(CFLAGS) $(CFLAGS) -o $@ gene.c \
		rand.o learn.o stats.o utility.o $(LIBS)
genetest: genetest.o learn.o rand.o stats.o utility.o types.h
	$(CC) -o $@ genetest.o learn.o rand.o stats.o utility.o $(LIBS)
histplot: histplot.o utility.o
	$(CC) $(CFLAGS) -o histplot histplot.o utility.o $(LIBS)
lint:	$(SRCS)
	lint $(CFLAGS) $(LIBS) $(SRCS)
player: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
testfind: testfind.o findscore.o utility.o
	$(CC) $(CFLAGS) -o $@ testfind.o findscore.o utility.o $(LIBS)
#
# Miscellaneous useful pseduo-makes
#
backup:
	rm -f backup.tar
	tar cvf backup.tar *.c *.h *.l rogomatic.6 makefile
	chmod ugo-w backup.tar
clean:
	rm -f $(ALL) *.CKP *.o datesub.c core

$(PUBDIR):
	mkdir $@
	chmod 777 $@

install: $(ALL) $(GAMDIR) $(GAMLIB) $(PUBDIR)
	cp rogomatic $(GAMDIR)/rogomatic
	cd $(GAMDIR); chmod 711 rogomatic
	cp player rgmplot datesub histplot gene $(PUBDIR)
	cd $(PUBDIR); chmod 711 player rgmplot datesub histplot gene

index: $(SRCS)
	ctags -x $(SRCS) > index
fluff: $(SRCS)
	lint *.c	| grep -v 'variable # of args' \
			| grep -v 'unused in function' \
			| grep -v 'used inconsistently' \
			| grep -v 'declared inconsistently' \
			| grep -v 'multiply declared' \
			> fluff
print: $(SRCS) $(HDRS)
	@echo $? > printit
dist: $(SRCS) $(HDRS) $(OTHERS) makefile rogomatic.6 README
	rm -rf $(PUBDIR)	
	mkdir $(PUBDIR)
	cp $(SRCS) $(HDRS) $(OTHERS) makefile rogomatic.6 README $(PUBDIR)
	chmod 0444 $(PUBDIR)/*
	du $(PUBDIR)

arms.o: types.h globals.h
command.o: types.h globals.h
database.o: types.h globals.h
debug.o: types.h globals.h install.h
explore.o: types.h globals.h
findscore.o: install.h
io.o: types.h globals.h termtokens.h install.h
mess.o: types.h globals.h
learn.o: types.h
ltm.o: types.h globals.h install.h
main.o: install.h termtokens.h types.h globals.h
monsters.o: types.h globals.h
pack.o: types.h globals.h
rand.o: rand.c
replay.o: types.h globals.h
rgmplot.o: rgmplot.c
rooms.o: types.h globals.h
scorefile.o: types.h globals.h install.h
search.o: types.h globals.h
setup.o: install.h termtokens.h
stats.o: types.h
strategy.o: types.h globals.h install.h
survival.o: types.h globals.h
tactics.o: types.h globals.h install.h
things.o: types.h globals.h
titlepage.o: titlepage.c
titler.o: titler.c
utility.o: install.h
worth.o: types.h globals.h
