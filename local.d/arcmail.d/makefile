#
#	$Header: /home/Vince/cvs/local.d/arcmail.d/makefile,v 1.5 2002-11-24 03:18:13 Vincent Exp $
#
ALL	= arcmail$(EXE)
MODEL	= -i -F 2000
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

lint:
	lint arcmail.c | tee lint.out

clean:
	rm -f $(ALL)
