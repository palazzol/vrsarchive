#
#	Makefile for table.c, method of producing a single terminal
#	table for nroff under USG UNIX.
#	Makefile written by Bruce Townsend (utcs!bnr-vpa!bruce)
#	and Ian Darwin (utcs!darwin!ian)
#	March 6, 1985.
#
TABLES	= tab1340 tab1340-ps
ALL	= $(TABLES) elbat
TERMDIR	= $(ROOT)/usr/lib/term
LINTTAB	= $(TABLES)
OPTIONS	= -O
MODEL	= -i
include $(HOME)/CONFIG


$(TABLES):: table.o
	@make $@.o
	$(CC) $(CFLAGS) table.o $@.o
	./$(DEFEXE) $@
	rm -f ./$(DEFEXE) $@.o

# This dummy_file should never exist -- it is a kludge to get Make to
elbat:	elbat.c
	$(CC) $(CFLAGS) elbat.c -o $@
	rm -f elbat.o

install: $(ALL) $(TERMDIR) $(BINDIR) $(MAN5)
	cp $(TABLES) $(TERMDIR)/
	cd $(TERMDIR); chmod 644 $(TABLES)
	cp elbat $(BINDIR)/
	cd $(BINDIR); chmod 711 elbat
	cp term.5 $(MAN5)/
	cd $(MAN5); chmod 644 term.5

$(TERMDIR):
	mkdir $@
	chmod 755 $@

lint:	table.c
	lint $(LFLAGS) table.c $(LINTTAB).c

clean:
	rm -f $(ALL) table.o
