#
#	$Header: /home/Vince/cvs/local.d/ctty.d/Makefile,v 1.3 1990-03-30 17:02:37 vrs Exp $
#
MODEL	= -M2em -F 2000
ALL	= ctty
include /etc/CONFIG

install: $(BINDIR) ctty
	cp ctty $(BINDIR)/ttype
	chown root $(BINDIR)/ttype
	chmod 4711 $(BINDIR)/ttype
	rm -f $(BINDIR)/tspeed
	ln $(BINDIR)/ttype $(BINDIR)/tspeed

clean:
	rm -f $(ALL)
