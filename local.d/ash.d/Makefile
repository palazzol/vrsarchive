#
#	$Header: /home/Vince/cvs/local.d/ash.d/Makefile,v 1.4 1990-03-30 17:02:19 vrs Exp $
#
ALL	= ash ash.has
OPTIONS	= -O
MODEL	= -i
include /etc/CONFIG

install: $(ALL) $(BINDIR) $(LIBDIR) $(MAN1)
	cp ash $(BINDIR)
	cd $(BINDIR); chmod 755 ash
	cp ash.help ash.inst ash.has $(LIBDIR)
	cd $(LIBDIR); chmod 644 ash.help ash.inst
	cd $(LIBDIR); chmod 711 ash.has
	cp ash.1 $(MAN1)/
	cd $(MAN1); chmod 644 ash.1
#
#	Special rule does fixups needed for current system
#
ash:	ash.sh
	sed '/^HELP/,/^FMT/c\
HELP=$(LIBDIR)/ash.help\
INST=$(LIBDIR)/ash.inst\
HAS=$(LIBDIR)/ash.has\
PAGE=$(PAGER)\
E2="\\c"\
FMT="pr -t -5"' $? > $@

ash.has: has.c
	$(CC) $(CFLAGS) has.c -o $@
	rm -f has.o
clean:
	rm -f $(ALL)
