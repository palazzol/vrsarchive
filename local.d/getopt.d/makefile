#
#	$Header: /home/Vince/cvs/local.d/getopt.d/makefile,v 1.4 2002-11-24 05:57:58 Vincent Exp $
#
ALL	= getopt$(EXE)
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
