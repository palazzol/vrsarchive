#
#	$Header: /home/Vince/cvs/local.d/espel.d/makefile,v 1.2 1987-02-15 15:52:41 vrs Exp $
#
ALL	= espel espel.hi spellin
OPTIONS	= -DLIB=\"$(LIBDIR)\"
include ../../CONFIG
#
#	PUBLIC is where to keep the public dictionaries
#
PUBLIC	= $(LIBDIR)
OFILES	= copy.o correct.o hashtab.o hi.o main.o minigrep.o

espel:	espel.sh
	sed '/^LIB/,/^PUBLIC/c\
LIB=$(LIBDIR)\
SPELLIN=$(LIBDIR)/spellin\
PUBLIC=$(PUBLIC)' $? > $@

espel.hi: $(OFILES)
	$(CC) $(CFLAGS) $(OFILES) -o espel.hi

install: $(ALL) $(BINDIR) $(LIBDIR) $(MAN1)
	cp espel $(BINDIR)
	chmod 755 $(BINDIR)/espel
	cp espel.help espel.hi spellin $(LIBDIR)
	cd $(LIBDIR); chmod 711 espel.hi spellin
	cd $(LIBDIR); chmod 644 espel.help
	cp /usr/lib/spell/hlista $(PUBLIC)/.spellht
	touch $(PUBLIC)/.spellwd
	chmod 666 $(PUBLIC)/.spellht $(PUBLIC)/.spellwd
	touch /usr/lib/spell/spellhist
	chmod 666 /usr/lib/spell/spellhist
	cp espel.l $(MAN1)
	chmod 644 $(MAN1)/espel.l

clean:
	rm -f $(ALL) $(OFILES)

spellin.o: spell.h
main.o: espel.h
hi.o: espel.h
hashtab.o: espel.h
correct.o: espel.h
copy.o:espel.h
minigrep.o: espel.h

