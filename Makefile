#
#	$Header: /home/Vince/cvs/Makefile,v 1.4 2002-11-23 20:51:55 Vincent Exp $
#
TODO	= jove net ucb vax
# Build lib first!
ALL	= lib local games
include CONFIG

install:
	@for i in $(ALL); do (cd $$i.d; make install); done

clean:
	@for i in $(ALL); do (cd $$i.d; make clean); done
