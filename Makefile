#
#	$Header: /home/Vince/cvs/Makefile,v 1.1 1987-02-15 19:30:01 vrs Exp $
#
TODO	= games jove lib net ucb vax
ALL	= local
include CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done
