#
#	$Header: /home/Vince/cvs/Makefile,v 1.3 1990-05-27 18:58:59 vrs Exp $
#
TODO	= jove net ucb vax
# Build lib first!
ALL	= lib local games
include CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done
