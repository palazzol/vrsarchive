#
#	$Header: /home/Vince/cvs/Makefile,v 1.2 1987-12-26 16:48:50 vrs Exp $
#
TODO	= lib net ucb vax
ALL	= games local
include CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done
