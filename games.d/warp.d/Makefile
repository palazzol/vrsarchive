#
# $Header: /home/Vince/cvs/games.d/warp.d/Makefile,v 1.4 1990-04-04 21:31:38 vrs Exp $
#
# $Log: not supported by cvs2svn $
# Version 1.3  90/03/27  17:28:26  vrs
# Move CONFIG to /etc
# 
# Version 1.2  87/07/26  15:04:43  vrs
# Use CONFIG
# 
# Version 1.1  87/07/26  11:29:19  vrs
# Initial version
# 
# Revision 7.0.1.1  86/12/12  16:54:03  lwall
# Frozen for net release.
# 
# Revision 7.0  86/10/08  15:11:42  lwall
# Split into separate files.  Added amoebas and pirates.
# 
#
public	= warp
private = smap.0 smap.1 smap.2 smap.3 smap.4 smap.5 smap.6 smap.7
ALL	= $(public) $(private)
OPTIONS	= -O -DPRIVLIB=\"$(privlib)\"
MODEL	= -M2m
include /etc/CONFIG
bin	= $(GAMDIR)
mansrc	= $(MAN6)
CHOWNER = $(SAFEUID)
privlib = $(GAMLIB)/warp
manext	= 6
NDIRC	= ndir.c
NDIRO	= ndir.o

util = Makefile makedepend makedir smap.0 smap.1 smap.2 smap.3 smap.4 smap.5 smap.6 smap.7 warp.doc
manpages = warp.man

sh = Makefile.SH makedepend.SH makedir.SH

h1 = bang.h init.h intrp.h move.h ndir.h
h2 = patchlevel.h object.h play.h score.h
h3 = sig.h term.h them.h us.h
h4 = util.h version.h warp.h weapon.h

h = $(h1) $(h2) $(h3) $(h4)

c1 = bang.c init.c intrp.c move.c
c2 = object.c play.c score.c
c3 = sig.c term.c them.c us.c
c4 = util.c version.c warp.c weapon.c

c = $(c1) $(c2) $(c3) $(c4) $(NDIRC)

obj1 = bang.o init.o intrp.o move.o
obj2 = object.o play.o score.o
obj3 = sig.o term.o them.o us.o
obj4 = util.o version.o warp.o weapon.o

obj = $(obj1) $(obj2) $(obj3) $(obj4) $(NDIRO)

lintflags = -phbvxac

add1 = Makefile.old 
add2 = bsd config.h config.sh eunice filexp
add3 = loc makedepend 
add4 = 
add5 = pdp11 warp usg v7

addedbyconf = $(add1) $(add2) $(add3) $(add4) $(add5)

# grrr
SHELL = /bin/sh

.c.o:
	$(CC) -c $(CFLAGS) $*.c

warp: $(obj)
	$(CC) $(CFLAGS) $(obj) $(LIBS) -o $@

# if a .h file depends on another .h file...
$(h):
	touch $@

install: $(ALL) $(bin)
	if test `pwd` != $(bin); then cp $(public) $(bin); fi
	cd $(bin); chmod 755 $(public); chown '$(CHOWNER)' $(public)
	chmod 4711 $(bin)/warp
	chmod 555 makedir
	./makedir `./filexp $(privlib)`
	-if test `pwd` != `./filexp $(privlib)`; then \
		cp $(private) `./filexp $(privlib)`; \
	fi
	cd `./filexp $(privlib)`; chmod 755 $(private); touch save.blurfl
	cd `./filexp $(privlib)`; chown '$(CHOWNER)' . $(private)
	-if test ! -f `./filexp $(privlib)/warp.news`; then \
		cp warp.news `./filexp $(privlib)`; \
	fi
	-if test `pwd` != $(mansrc); then \
		for page in $(manpages); do \
			cp $$page $(mansrc)/`basename $$page .man`.$(manext); \
		done; \
	fi

clean:
	rm -f $(ALL) warp.*top warp.log sm core *.o

realclean:
	rm -f warp *.o core $(addedbyconf) $(util)

# The following lint has practically everything turned on.  Unfortunately,
# you have to wade through a lot of mumbo jumbo that can't be suppressed.
# If the source file has a /*NOSTRICT*/ somewhere, ignore the lint message
# for that spot.

lint:
	lint $(lintflags) $(defs) $(c) > warp.fuzz

smap.0: smp.0 sm
	./sm <smp.0 >smap.0
smap.1: smp.1 sm
	./sm <smp.1 >smap.1
smap.2: smp.2 sm
	./sm <smp.2 >smap.2
smap.3: smp.3 sm
	./sm <smp.3 >smap.3
smap.4: smp.4 sm
	./sm <smp.4 >smap.4
smap.5: smp.5 sm
	./sm <smp.5 >smap.5
smap.6: smp.6 sm
	./sm <smp.6 >smap.6
smap.7: smp.7 sm
	./sm <smp.7 >smap.7
sm: sm.c
	cc sm.c -o $@

depend: makedepend
	makedepend

clist:
	echo $(c) | tr ' ' '\012' >.clist

hlist:
	echo $(h) | tr ' ' '\012' >.hlist

shlist:
	echo $(sh) | tr ' ' '\012' >.shlist

# AUTOMATICALLY GENERATED MAKE DEPENDENCIES--PUT NOTHING BELOW THIS LINE
bang.o: bang.c
init.o: init.c
intrp.o: intrp.c
move.o: move.c
object.o: object.c
play.o: play.c
score.o: score.c
sig.o: sig.c
term.o: term.c
them.o: them.c
us.o: us.c
util.o: util.c
version.o: version.c
warp.o: warp.c
weapon.o: weapon.c
ndir.o: ndir.c
warp.h: /usr/include/stdio.h
warp.h: /usr/include/signal.h
warp.h: /usr/include/ctype.h
warp.h: /usr/include/sys/types.h
warp.h: /usr/include/sys/stat.h
warp.h: /usr/include/errno.h
warp.h: /usr/include/sys/ioctl.h
warp.h: /usr/include/sys/timeb.h
Makefile: Makefile.SH config.sh ; /bin/sh Makefile.SH
makedepend: makedepend.SH config.sh ; /bin/sh makedepend.SH
makedir: makedir.SH config.sh ; /bin/sh makedir.SH
# WARNING: Put nothing here or make depend will gobble it up!
