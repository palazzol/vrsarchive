#	Hack or Quest Makefile.
#	SCCS Id: @(#)Makefile.xenix	1.4	87/08/08

# make NetHack
GAME	= nethack
ALL	= $(GAME) $(VARAUX)
GAMEUID = $(SAFEUID)
GAMEGRP = $(SAFEGRP)
HACKDIR	= $(GAMLIB)/$(GAME)dir
SAVEDIR	= $(HACKDIR)/save
OPTIONS	= -DHACKDIR=\"$(HACKDIR)\"
MODEL	= -M2lt50 -F 2000
include /etc/CONFIG

# if you are cross-compiling (eg. from Xenix into a Dos environment)
# you will have to redefine these filenames.
MAIN = unixmain.c
TTY  = unixtty.c
UNIX = unixunix.c

HACKCSRC = apply.c bones.c decl.c\
	cmd.c do.c dothrow.c do_name.c do_wear.c dog.c dogmove.c eat.c end.c\
	engrave.c fight.c fountain.c hack.c invent.c ioctl.c lev.c main.c\
	makemon.c mhitu.c mklev.c mkmaze.c mkobj.c mkshop.c\
	mon.c monmove.c monst.c o_init.c objnam.c options.c\
	pager.c polyself.c potion.c pray.c pri.c prisym.c read.c rip.c\
	rumors.c save.c search.c shk.c shknam.c sit.c spell.c steal.c\
	termcap.c timeout.c topl.c topten.c track.c trap.c tty.c unix.c\
	u_init.c vault.c wield.c wizard.c worm.c worn.c write.c zap.c\
	version.c rnd.c alloc.c

CSOURCES = $(HACKCSRC) makedefs.c

HSOURCES = hack.h mfndpos.h config.h edog.h eshk.h extern.h\
	flag.h func_tab.h gold.h mkroom.h monst.h obj.h\
	objclass.h objects.h permonst.h rm.h spell.h trap.h wseg.h you.h

SOURCES = $(CSOURCES) $(HSOURCES)

AUX = help hh nethack.6 nethack.sh
VARAUX = data rumors

DISTR = $(SOURCES) $(AUX) $(VARAUX) README.OLD README\
	Makefile.unix Makefile.xenix Makefile.pc Make.ini

HOBJ = apply.o bones.o decl.o cmd.o do.o dothrow.o do_name.o do_wear.o dog.o\
	dogmove.o eat.o end.o engrave.o fight.o fountain.o hack.o\
	invent.o ioctl.o lev.o main.o makemon.o mhitu.o mklev.o\
	mkmaze.o mkobj.o mkshop.o mon.o monmove.o monst.o\
	o_init.o objnam.o options.o pager.o polyself.o potion.o pray.o\
	pri.o prisym.o read.o rip.o rumors.o save.o search.o shk.o shknam.o\
	sit.o spell.o steal.o termcap.o timeout.o topl.o topten.o track.o\
	trap.o tty.o unix.o u_init.o vault.o wield.o wizard.o worm.o worn.o\
	write.o zap.o version.o rnd.o alloc.o

$(GAME): main.o tty.o unix.o $(HOBJ)
	@echo "Loading ..."
	@cc $(LFLAGS) -o $(GAME) $(CRT0) $(HOBJ) $(TERMLIB) $(LIBS)

makedefs:	makedefs.c alloc.o config.h
	cc ${CFLAGS} -o makedefs alloc.o makedefs.c

RUMORFILES= rumors.base rumors.kaa rumors.mrx

rumors:	$(RUMORFILES) makedefs
	chmod u+w $(RUMORFILES)
	./makedefs -r

data:	data.base makedefs
	chmod u+w $@.base
	./makedefs -d

date.h:	$(SOURCES) makedefs
	chmod u+w $@
	./makedefs -D

trap.h:	makedefs
	chmod u+w $@
	./makedefs -t

onames.h:	makedefs objects.h
	./makedefs -o

main.c:	$(MAIN)
	cp $? $@
	chmod u+w $@

tty.c:	$(TTY)
	cp $? $@
	chmod u+w $@

unix.c:	$(UNIX)
	cp $? $@
	chmod u+w $@

lint:
# lint cannot have -p here because (i) capitals are meaningful:
# [Ww]izard, (ii) identifiers may coincide in the first six places:
# doweararm() versus dowearring().
# _flsbuf comes from <stdio.h>, a bug in the system libraries.
	@echo lint -axbh -DLINT ...
	@lint -axbh -DLINT $(HACKCSRC) | sed '/_flsbuf/d'


diff:
	@- for i in $(SOURCES) $(AUX) ; do \
		cmp -s $$i $D/$$i || \
		( echo diff $D/$$i $$i ; diff $D/$$i $$i ; echo ) ; done

distribution: Makefile
	@- for i in READ_ME $(SOURCES) $(AUX) Makefile date.h onames.h\
		; do \
		cmp -s $$i $D/$$i || \
		( echo cp $$i $D ; cp $$i $D ) ; done
# the distribution directory also contains the empty files perm and record.

$(HACKDIR) $(SAVEDIR):
	mkdir $@
	chown $(GAMEUID) $@
	chgrp $(GAMEGRP) $@
	chmod 755 $@

install: $(ALL) $(GAMDIR) $(GAMLIB) $(HACKDIR) $(SAVEDIR)
	rm -f $(HACKDIR)/bones* $(SAVEDIR)/*
	cd $(HACKDIR); touch perm record
	cd $(HACKDIR); chmod 644 perm record
	cp help hh rumors data $(HACKDIR)
	cd $(HACKDIR); chmod 644 help hh rumors data
	sed -e 's/#!/: /' -e 's:^HACKDIR=.*:HACKDIR=$(HACKDIR):' $(GAME).sh >$(GAMDIR)/$(GAME)
	chmod 755 $(GAMDIR)/$(GAME)
	cp $(GAME) $(HACKDIR)/$(GAME)
	chown $(GAMEUID) $(GAMDIR)/$(GAME) $(HACKDIR)/*
	chgrp $(GAMEGRP) $(GAMDIR)/$(GAME) $(HACKDIR)/*
	chmod 04755 $(HACKDIR)/$(GAME)
	cp nethack.6 $(MAN6)
	cd $(MAN6); chmod 644 nethack.6

clean:
	rm -f $(ALL) *.o a.out core makedefs main.c tty.c unix.c onames.h

bones.o:  hack.h
decl.o:  hack.h mkroom.h
end.o:  hack.h
hack.o:  hack.h
ioctl.o:  config.h
o_init.o:  config.h objects.h onames.h
objnam.o:  hack.h
u_init.o:  hack.h
version.o:  hack.h date.h
apply.o:  hack.h edog.h mkroom.h
cmd.o:  hack.h func_tab.h
do.o:  hack.h
dothrow.o:  hack.h
do_name.o:  hack.h
do_wear.o:  hack.h
dog.o:  hack.h edog.h mkroom.h
dogmove.o:  hack.h mfndpos.h
eat.o:  hack.h
engrave.o:  hack.h
fight.o:  hack.h
fountain.o:  hack.h
invent.o:  hack.h wseg.h
lev.o:  hack.h mkroom.h wseg.h
makemon.o:  hack.h
mhitu.o:  hack.h
mklev.o:  hack.h mkroom.h
mkmaze.o:  hack.h mkroom.h
mkobj.o:  hack.h
mkshop.o:  hack.h mkroom.h eshk.h
mon.o:  hack.h mfndpos.h
monmove.o:  hack.h mfndpos.h
monst.o:  hack.h eshk.h
options.o:  config.h hack.h
pager.o:  hack.h
polyself.o: hack.h
potion.o:  hack.h
pray.o: hack.h
pri.o:  hack.h
prisym.o:  hack.h wseg.h
read.o:  hack.h
rip.o:  hack.h
rumors.o:  hack.h
save.o:  hack.h
search.o:  hack.h
shk.o:  hack.h mfndpos.h mkroom.h eshk.h
shknam.o:  hack.h
sit.o:  hack.h
spell.o:  hack.h
steal.o:  hack.h
termcap.o:  config.h flag.h
timeout.o:  hack.h
topl.o:  hack.h
topten.o:  hack.h
track.o:  hack.h
trap.o:  hack.h mkroom.h
vault.o:  hack.h mkroom.h
wield.o:  hack.h
wizard.o:  hack.h
worm.o:  hack.h wseg.h
worn.o:  hack.h
write.o:  hack.h
zap.o:  hack.h
extern.h: config.h
	touch extern.h
hack.h:  config.h objclass.h monst.h gold.h trap.h obj.h flag.h rm.h permonst.h spell.h onames.h extern.h you.h
	touch hack.h
objects.h:  config.h objclass.h
	touch objects.h
main.o:	hack.h
tty.o:	extern.h
unix.o:	hack.h mkroom.h
