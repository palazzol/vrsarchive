#
#	$Header: /home/Vince/cvs/local.d/index.d/makefile,v 1.6 2002-11-24 17:15:03 vrs Exp $
#
ALL	= index$(EXE)
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) index.o

index$(EXE):	index.o
	$(CC) $(CFLAGS) $(LDFLAGS) $? $(LEXLIB) -o $@
