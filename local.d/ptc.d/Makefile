#
#	@(#)Makefile	1.5 Date 87/05/09
#
MODEL	= -M2l
OPTIONS	= -O
ALL	= test
include /etc/CONFIG
FILES	= makefile ptc.p ptc.c ptc doc.ms doc.out ptc.man man.out
#
#	Build the bootstrap translator
#
boot:	boot.o
	$(CC) $(CFLAGS) -o $@ boot.o
#
#	Make the Pascal version
#
cptc.c:	ptc.p boot
	./boot < ptc.p > tmp.c
	mv tmp.c cptc.c

ptc:	cptc.o
	cc -O -o $@ cptc.o
#
#	Compare reults from Pascal and C versions
#
test.c:	cptc.c ptc
	ptc < ptc.p > tmp.c
	mv tmp.c test.c

test:	cptc.c test.c
	@diff cptc.c test.c

install: ptc ptc.man $(BINDIR)
	cp ptc $(BINDIR)
	cd $(BINDIR); chmod 711 ptc
	cp ptc.man $(MAN1)/ptc.1
	chmod 644 $(MAN1)/ptc.1

clean:
	rm -f tmp.c boot.o boot cptc.c cptc.o ptc test.c
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
