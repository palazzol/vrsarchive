#
#	$Header: /home/Vince/cvs/local.d/read20.d/makefile,v 1.6 2002-11-24 05:50:40 Vincent Exp $
#
ALL	= read20$(EXE)
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG
CFILES	= read20.c

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp read20.1 $(MAN1)
	cd $(MAN1); chmod 644 read20.1

read20.o: dump.h

clean:
	rm -f $(ALL) $(CFILES:.c=.o)

tags: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)
