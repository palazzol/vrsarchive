#
#	@(#)Makefile	1.5 Date 87/05/09
#
MODEL	= -M2l
OPTIONS	= -O
ALL	= test
include $(HOME)/CONFIG
FILES	= makefile ptc.p ptc.c ptc$(EXE) doc.ms doc.out ptc.man man.out
#
#	Build the bootstrap translator
#
boot$(EXE):	boot.o
	$(CC) $(CFLAGS) -o $@ boot.o
#
#	Make the Pascal version
#
cptc.c:	ptc.p boot
	./boot$(EXE) < ptc.p > tmp.c
	mv tmp.c cptc.c

ptc$(EXE):	cptc.o
	$(CC) -O -o $@ cptc.o
#
#	Compare reults from Pascal and C versions
#
test.c:	cptc.c ptc
	./ptc$(EXE) < ptc.p > tmp.c
	mv tmp.c test.c

test:	cptc.c test.c
	@diff cptc.c test.c

install: ptc ptc.man $(BINDIR)
	cp ptc$(EXE) $(BINDIR)
	cd $(BINDIR); chmod 711 ptc$(EXE)
	cp ptc.man $(MAN1)/ptc.1
	chmod 644 $(MAN1)/ptc.1

clean:
	rm -f tmp.c boot.o boot$(EXE) cptc.c cptc.o ptc$(EXE) test.c
#
#	Documentation.
#
doc.out:	doc.ms
		tbl doc.ms | nroff -ms >$@

man.out:	ptc.man
		nroff -man ptc.man >$@
#
#	Dependencies
#
boot.o: globals.h
