#
#	$Header: /home/Vince/cvs/local.d/arcmail.d/makefile,v 1.4 2002-11-23 07:48:55 Vincent Exp $
#
ALL	= arcmail
MODEL	= -i -F 2000
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

lint:
	lint arcmail.c | tee lint.out

clean:
	rm -f $(ALL)
