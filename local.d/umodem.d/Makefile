#
#	$Header: /home/Vince/cvs/local.d/umodem.d/Makefile,v 1.4 2002-11-24 18:02:11 vrs Exp $
#
ALL	= umodem$(EXE)
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
