#
#	Makefile for rogue - Revised for Xenix
#	@(#)Makefile	6.7 (Berkeley) 10/3/84
#
#
#	-DWIZARD	Compile in "wizard mode" (reguires large machine)
#	-DDUMP		Allow core dumps (bad idea for production)
#
ALL	= rogue$(EXE) scedit$(EXE)
OPTIONS	= -UWIZARD -UDUMP -O
MODEL	= -M2m -F 1800 $(STRIP)
include $(HOME)/CONFIG
#
#	Where the scoreboard should be kept.
#
SCOREFILE= $(GAMLIB)/rogue_roll
SF	= -DSCOREFILE='"$(SCOREFILE)"'
#
#	Use the kernel to namelist to get load averages.  This works only
#	on Berkeley kernels (needs _avenrun).
#
#NAMELIST= /vmunix
#NL	= -DNAMELIST #='"$(NAMELIST)"'
#MACHDEP = -DMAXLOAD=40 -DLOADAV -DCHECKTIME=4
#
#	Use a raw count of users rather than load measured by the kernel
#	to kick users off.  This is faster and more portable than using
#	_avenrun, but makes it hard to play rogue if idle users don't log
#	off.
#
#MACHDEP = -DMAXUSERS=4 -DUCOUNT -DCHECKTIME=4 -DUTMP='"/etc/utmp"'

PROFLAGS= -pg -O
PLIBS	= -lcurses -ltermlib	# Profiling needs small model in Xenix
VGRIND	= /usr/ucb/vgrind

SHELL	= /bin/sh
HDRS	= rogue.h extern.h score.h
DOBJS	= vers.o extern.o armor.o chase.o command.o daemon.o daemons.o \
	  fight.o init.o io.o list.o main.o misc.o monsters.o move.o \
	  new_level.o options.o pack.o passages.o potions.o rings.o rip.o \
	  rooms.o save.o scrolls.o sticks.o things.o weapons.o wizard.o \
	  m_alloc.o # newwin.o
MISC	= xstr.c Makefile Dist.fmt prob.c findpw.c
OBJS	= $(DOBJS) mach_dep.o
POBJS	= vers.po extern.po armor.po chase.po command.po daemon.po daemons.po \
	  fight.po init.po io.po list.po main.po misc.po monsters.po move.po \
	  new_level.po options.po pack.po passages.po potions.po rings.po \
	  rip.po rooms.po save.po scrolls.po sticks.po things.po weapons.po \
	  wizard.po mach_dep.po m_alloc.po
CFILES	= vers.c extern.c armor.c chase.c command.c daemon.c daemons.c \
	  fight.c init.c io.c list.c main.c misc.c monsters.c move.c \
	  new_level.c options.c pack.c passages.c potions.c rings.c rip.c \
	  rooms.c save.c scrolls.c sticks.c things.c weapons.c wizard.c \
	  mach_dep.c scedit.c scmisc.c m_alloc.c

BINARY	= distr.out

.SUFFIXES: .po

.c.po:
	@echo $(CC) -c $(PROFLAGS) $*.c
	@cc -E $(PROFLAGS) $*.c | xstr -c -
	@cc -c $(PROFLAGS) x.c
	@mv x.o $*.po

#.c.o:
#	@echo $(CC) -c $(CFLAGS) $*.c
#	@cc -E $(CFLAGS) $*.c | xstr -c -
#	@cc -c $(CFLAGS) -NT $* x.c
#	@mv x.o $*.o
#
#rogue: $(HDRS) $(OBJS) xs.o
#	@rm -f x.c
#	$(CC) $(CFLAGS) xs.o $(OBJS) $(LIBS) -o $@
#	size rogue

rogue$(EXE): $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@
	size $@

vers.o:
	$(CC) $(CFLAGS) -c vers.c

mach_dep.o:
	$(CC) $(CFLAGS) $(SF) $(NL) $(MACHDEP) -c mach_dep.c

xs.o: strings
	xstr
	$(CC) $(CFLAGS) -c xs.c

install: $(ALL)
	cp rogue$(EXE) $(GAMDIR)/rogue$(EXE)
	chown $(SAFEUID) $(GAMDIR)/rogue$(EXE)
	chmod 4711 $(GAMDIR)/rogue$(EXE)
	touch $(SCOREFILE)
	(echo "w"; echo "q") | ./scedit$(EXE) >/dev/null
	chown $(SAFEUID) $(SCOREFILE)

distr: rmmach distmod.o mach_dep.o
	@rm -f x.c
	$(CC) -s $(CFLAGS) -o =bin/$(BINARY) distmod.o mach_dep.o $(LIBS)
	size =bin/$(BINARY)

rmmach:
	rm -f mach_dep.o

distmod.o: $(DOBJS) xs.o
	@rm -f x.c
	-$(LD) -r -x -o distmod.o xs.o $(DOBJS)

findpw$(EXE): findpw.c
	$(CC) -s -o findpw findpw.c

prob$(EXE): prob.o extern.o xs.o
	$(CC) $(CFLAGS) -o prob prob.o extern.o xs.o

prob.o: prob.c rogue.h
	$(CC) $(CFLAGS) -c prob.c

scedit$(EXE): scedit.o scmisc.o vers.o
	$(CC) $(CFLAGS) -o scedit vers.o scedit.o scmisc.o -lcurses

scmisc.o scedit.o:
	$(CC) $(CFLAGS) -c $(SF) $*.c

p.out:	$(HDRS) $(POBJS) xs.po
	@rm -f x.c
	$(CC) $(PROFLAGS) $(CFLAGS) -o p.out xs.po $(POBJS) $(LIBS)
	size p.out

vers.po:
	@echo $(CC) -c $(PROFLAGS) vers.c
	@ln vers.c x.c
	@cc -c $(PROFLAGS) $(SF) x.c
	@mv x.o vers.po
	@rm -f x.c

mach_dep.po:
	$(CC) -c $(PROFLAGS) $(SF) $(NL) $(MACHDEP) mach_dep.c
	mv mach_dep.o mach_dep.po

xs.po: strings
	xstr
	@echo $(CC) -c $(PROFLAGS) xs.c
	@mv xs.c x.c
	@$(CC) -c $(PROFLAGS) -ND STRINGS x.c
	@mv x.o xs.po

tags: $(HDRS) $(CFILES)
	ctags -u $?
	ed - tags < :ctfix
	sort tags -o tags

lint:
	lint $(LIBS) $(CFLAGS) $(MACHDEP) $(SF) $(NL) $(CFILES) > linterrs 2>&1

clean:
	rm -f $(ALL) $(POBJS) $(OBJS) core p.out strings make.out \
	 	? rogue.tar vgrind.* x.c x.o xs.c xs.o linterrs findpw \
		distmod.o xs.po scmisc.o scedit.o

count:
	wc -l $(HDRS) $(CFILES)

realcount:
	cc -E $(CFILES) | ssp - | wc -l

update:
	ar uv .SAVE $(CFILES) $(HDRS) Makefile

dist:
	cp $(CFILES) $(HDRS) Makefile /ra/csr/toy/_dist

xtar: $(HDRS) $(CFILES) $(MISC)
	rm -f rogue.tar
	tar cf rogue.tar $? :ctfix
	touch xtar

vgrind:
	@csh $(VGRIND) -t -h "Rogue Version 10/3/84" $(HDRS) *.c > vgrind.out
	@ctags -v $(HDRS) *.c > index
	@csh $(VGRIND) -t -x index > vgrind.out.tbl

cfiles: $(CFILES)
