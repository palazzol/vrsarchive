#
#	$Header: /home/Vince/cvs/local.d/getopt.d/makefile,v 1.3 2002-11-23 07:49:07 Vincent Exp $
#
ALL	= getopt
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG
DOCS	= getopt.1

install: $(ALL) $(BINDIR) $(DOCS) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp $(DOCS) $(MAN1)
	cd $(MAN1); chmod 644 $(DOCS)

clean:
	rm -f $(ALL)
