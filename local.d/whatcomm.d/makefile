#
#	$Header: /home/Vince/cvs/local.d/whatcomm.d/makefile,v 1.2 1987-02-15 16:03:53 vrs Exp $
#
ALL	= whatcomm
MODEL	= -i
OPTIONS	= -DFILENAME=\"$(LIBDIR)/whatlist\"
include ../../CONFIG

install: $(ALL) $(BINDIR) 
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp whatlist $(LIBDIR)
	cd $(LIBDIR); chmod 644 whatlist
	cp whatcomm.1 $(MAN1)
	cd $(MAN1); chmod 644 whatcomm.1

lint:
	lint whatcomm.c | tee lint.out

clean:
	rm -f $(ALL)
