#
#	$Header: /home/Vince/cvs/local.d/index.d/makefile,v 1.5 2002-11-24 06:02:33 Vincent Exp $
#
ALL	= index$(EXE)
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) index.o

index:	index.o
	$(CC) $(CFLAGS) $(LDFLAGS) $? $(LEXLIB) -o $@
