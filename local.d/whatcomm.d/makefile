#
#	$Header: /home/Vince/cvs/local.d/whatcomm.d/makefile,v 1.5 2002-11-24 06:13:33 Vincent Exp $
#
ALL	= whatcomm$(EXE)
MODEL	= -i
OPTIONS	= -DFILENAME=\"$(LIBDIR)/whatlist\"
include $(HOME)/CONFIG

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
