#
#	$Header: /home/Vince/cvs/local.d/include.d/makefile,v 1.5 2002-11-24 05:59:25 Vincent Exp $
#
ALL	= include$(EXE)
MODEL	= -i
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

lint:
	lint $(CFLAGS) include.c | tee lint.out

clean:
	rm -f $(ALL)
