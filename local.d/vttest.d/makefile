#
#	$Header: /home/Vince/cvs/local.d/vttest.d/makefile,v 1.5 2002-11-24 06:12:49 Vincent Exp $
#
ALL	= vttest$(EXE)
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
