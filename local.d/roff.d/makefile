#
#	$Header: /home/Vince/cvs/local.d/roff.d/makefile,v 1.4 2002-11-23 07:51:56 Vincent Exp $
#
ALL	= roff
OPTIONS	= -O
include $(HOME)/CONFIG

lint:
	lint roff.c | tee lint.out

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp roff.1 $(MAN1)
	cd $(MAN1); chmod 644 roff.1

clean:
	rm -f $(ALL) lint.out
