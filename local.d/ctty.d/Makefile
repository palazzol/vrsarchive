#
#	$Header: /home/Vince/cvs/local.d/ctty.d/Makefile,v 1.2 1987-02-15 15:52:10 vrs Exp $
#
MODEL	= -M2em -F 2000
ALL	= ctty
include ../../CONFIG

install: $(BINDIR) ctty
	cp ctty $(BINDIR)/ttype
	chown root $(BINDIR)/ttype
	chmod 4711 $(BINDIR)/ttype
	rm -f $(BINDIR)/tspeed
	ln $(BINDIR)/ttype $(BINDIR)/tspeed

clean:
	rm -f $(ALL)
