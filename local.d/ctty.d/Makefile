#
#	$Header: /home/Vince/cvs/local.d/ctty.d/Makefile,v 1.4 2002-11-23 07:49:04 Vincent Exp $
#
MODEL	= -M2em -F 2000
ALL	= ctty
include $(HOME)/CONFIG

install: $(BINDIR) ctty
	cp ctty $(BINDIR)/ttype
	chown root $(BINDIR)/ttype
	chmod 4711 $(BINDIR)/ttype
	rm -f $(BINDIR)/tspeed
	ln $(BINDIR)/ttype $(BINDIR)/tspeed

clean:
	rm -f $(ALL)
