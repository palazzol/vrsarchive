#
#	$Header: /home/Vince/cvs/local.d/ash.d/Makefile,v 1.2 1987-02-15 15:48:59 vrs Exp $
#
ALL	= ash
include ../../CONFIG

install: $(ALL) $(BINDIR) $(LIBDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 755 $(ALL)
	cp ash.help ash.inst $(LIBDIR)
	cd $(LIBDIR); chmod 644 ash.help ash.inst
	cp ash.1 $(MAN1)
	cd $(MAN1); chmod 644 ash.1
#
#	Special rule does fixups needed for current system
#
ash:	ash.sh
	sed '/^PAGE/,/^MAINT/c\
PAGE=$(PAGER)\
HELP=$(LIBDIR)/ash.help\
INST=$(LIBDIR)/ash.inst\
MAINT=root' $? > $@

clean:
	rm -f $(ALL)
