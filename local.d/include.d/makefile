#
#	$Header: /home/Vince/cvs/local.d/include.d/makefile,v 1.2 1987-02-15 15:53:36 vrs Exp $
#
ALL	= include
MODEL	= -i
include ../../CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

lint:
	lint $(CFLAGS) include.c | tee lint.out

clean:
	rm -f $(ALL)
