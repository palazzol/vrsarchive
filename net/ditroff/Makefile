DESTDIR=$R/usr/bin
SEPID=
OPT=-O
HACKS=-DENVIRONMENT -DDESCHASNAME -DPAPERSIZEREG -DLOCALWARN -DRIBSFIX\
-DCHECKNFONTS -DMOREPREFONTS -DMOVEAFTERDRAW -DMAKEDEVITDEF \
-UUSEDOTFORTMPIFA -DZEROLINES -DTOOLATEFIX -DREPORTERRS -DWARNLEVELREG\
-DLINENUMFIX -DPROTECTBUILTINS -DEVREG -DLOTSOFENVS -DROTATEPAGE -DUSEFONTPATH\
-DFONTFAMILIES -DNOOPTARGFIX -UANYBASESTR -DDXLIM=200 -DGREMLIN -DBLANKSMACRO \
-DBUILTINCONTROLFIX -DLONGOVFIX -DALIGNFIXES -UBSD42 -DTCHARTOS -DNHEX=256\
-DSKIPFIX -DTESTFILES
# additional flags for Makedev
MAKEDEVHACKS=-DCHECKDESCFILES
# -DCOMMENT ------- various comments added by req.  No code difference.
# -DENVIRONMENT --- look at "TYPESETTER" for -Txxx	- req
# -DDESCHASNAME --- DESC.out contains the name of the typesetter - req
# -DPAPERSIZEREG -- read-only registers .W and .M for paper width/length - req
# -DLOCALWARN ----- warn if local features are used	- req
# -DRIBSFIX ------- R, I, B, S, X are the same as 1, 2, 3, smnt, 4 resp. --
#  ('X' is Bold Italic)	- req
# -DCHECKNFONTS --- check the number of pre-loaded fonts before loading them,
#  instead of simply core-dumping if there were too many...
# -DMOREPREFONTS -- allow more than 10 preloaded fonts.  It's a good idea to
#  set CHECKNFONTS with this too, as NFONTS is defined in lots of places...
# -DMOVEAFTERDRAW - put out a move function after a DRAW (from \D) command.
#  This is because the various filters seem to disagree about where we should
#  end up after drawing a spline or line or...		- req
# -DMAKEDEVITDEF -- Makedev includes tdef.h to ensure synchronisation of magic
#  numbers...	- req
# -UUSEDOTFORTMPIFA --- as distributed, if troff's name begins with "a", it
#  uses the current directory for tmp files (presumably for testing). - req
# -DZEROLINES ----- cope with \l'3i x' where x is a zero-width character - req
# -DREPORTERRS ---- give more helpful (and complete) error messages	- req
# -DTOOLATEFIX ---- don't let integer overflow in total # chars o/p cause
#  the tty flags to be corrupted (!)	- req
# -DWARNLEVELREG -- use a troff number register (wl) to store the warning level
#  for warnings about local features.  Then it can be turned on/off from within
#  troff (without producing a warning!)		- req
# -DPROTECTBUILTINS --- give a warning message if a built-in request is
#  changed (eg. ".de rm" or ".rm rm").  There doesn't seem any advantage in
#  actually forbidding this.
# -DEVREG --------- The .e read-only number register = current environment #
# -DLOTSOFENVS ---- allow more (currently 7) environments
# -DROTATEPAGE ---- add .rp n  to rotate the page through 90 degrees
# -DUSEFONTPATH --- fontfile is a colon-delimited path (eg. with -F option)
# -DFONTFAMILIES -- allow ".ff font-family"
# -DNOOPTARGFIX --- fix "troff -F" etc. (need an argument, but wasn't checked)
# -DDXLIM=200 ----- limit for number of pairs of points in \D'~ x y ...' (was
#  wired in as 50)     - req
# -DGREMLIN ------- allow ucb gremlin features
# -DANYBASESTR ---- allow arbitrary baseline strings (not finished yet)
# -DCOLOUR -------- allow colour to be specified (not done yet)
# -DBLANKSMACRO --- allow user-defined request called on leading whitespace
# -DBUILTINCONTROLFIX - control() calls built-ins as well as others for
# .em or .bm -- req
# -DLONGOVFIX ----- produce a warning if \o'xxxxxx' overflows the buffer
#  (instead of dumping core)
# -DALIGNFIXES ---- fixes to make troff slightly more careful about pointer
#  alignment & sizeof()...  - req
# -UBSD42 --------- op sys is Berkeley 4.2BSD.  Most fixes taken from 4.2bsd
#  troff and compared by hand.  This might well not be good enough.  - req
# -DTCHARTOS ------ use tchartos() to provide a printable representation of
#  a tchar.  One day this should clearly be combined with the output routines
#  of t10.c -- req.
# -DSKIPFIX ------- make skip() passe over tabs as well as spaces -- req
# -DTESTFILES ----- allow ".if r file" (true if can read file)
#
FBASE=$R/usr/lib/troff/descs
CFLAGS=$(OPT) -g -DFONTFILE=\"$(FBASE)\" -DFONTPATH=\"$(FBASE)/dev%s\" $(HACKS)
CC=cc
MAKE=make
CLEAN=clean

CFILES=n1.c n2.c n3.c n4.c n5.c t6.c n7.c n8.c n9.c t10.c ni.c nii.c hytab.c suftab.c local.c
HFILES=tdef.h v.h d.h s.h tw.h dev.h
TFILES=n1.o n2.o n3.o n4.o n5.o t6.o n7.o n8.o n9.o t10.o ni.o nii.o hytab.o suftab.o local.o
DRIVERS=daps dva dcat d202 dcan d8400 dxerox

all:	titroff makedev $(DRIVERS)

titroff: $(TFILES)
	$(CC) -o titroff $(TFILES)

install:  all
	-strip titroff
	cp titroff makedev $R/usr/bin
	cp $R/usr/bin/titroff $R/usr/bin/ditroff
	-tar fc - dev* | (cd $R/usr/lib/troff/descs; tar fxv -)
	cp $(DRIVERS) $R/usr/lib/troff/drivers
	$(MAKE) $(CLEAN)

local.o:	ext.h local.h tdef.h
n1.o:	tdef.h d.h v.h tw.h s.h local.h
n2.o:	tdef.h d.h v.h tw.h s.h
n3.o:	tdef.h d.h v.h tw.h s.h
n4.o:	tdef.h d.h v.h tw.h s.h dev.h
n5.o:	tdef.h d.h v.h tw.h s.h
n6.o:	tdef.h d.h v.h tw.h s.h
t6.o:	tdef.h d.h v.h tw.h s.h dev.h
n7.o:	tdef.h d.h v.h tw.h s.h
n8.o:	tdef.h d.h v.h tw.h s.h
n9.o:	tdef.h d.h v.h tw.h s.h
n10.o:	tdef.h d.h v.h tw.h s.h
t10.o:	tdef.h d.h v.h tw.h s.h dev.h
ni.o:	tdef.h d.h v.h tw.h s.h
nii.o:	tdef.h d.h v.h tw.h s.h local.h

hytab.o:	hytab.c

suftab.o:	suftab.c


list:
	@pr TODO n[1-6].c t6.c n[7-9].c [nt]10.c ni.c nii.c *.h

lint:
	lint -phc $(CFILES)

lib:	titroff
	cp titroff $R/usr/bin/titroff

dxerox:	dxerox.c
	$(CC) $(CFLAGS) -o dxerox dxerox.c -lm

dva.o:	dva.c
	$(CC) $(CFLAGS) -c -DDEBUG dva.c -lm

dva:	dva.o draw.o dev.h
	$(CC) $(CFLAGS) -o dva dva.o draw.o -lm

dcat:	dcat.o draw.o dev.h
	$(CC) $(CFLAGS) -o dcat dcat.o draw.o -lm

d202:	d202.o draw.o dev.h
	$(CC) $(CFLAGS) -O -o d202 d202.o draw.o -lm

d8400:	d8400.o draw.o dev.h
	$(CC) $(CFLAGS) -O -o d8400 d8400.o draw.o -lm

jc:	jc.o draw.o dev.h
	$(CC) $(CFLAGS) -O -o jc jc.o draw.o -lm

dcan:	dcan.o draw.o dev.h
	$(CC) $(CFLAGS) -O -o dcan dcan.o draw.o -lm

dsort:	dsort.o dev.h
	$(CC) $(CFLAGS) -i -O -o dsort dsort.o

daps:	daps.o draw.o aps.h dev.h
	$(CC) $(CFLAGS) -o daps daps.o draw.o -lm

tc:	tc.o draw.o dev.h
	$(CC) $(CFLAGS) -O -o tc tc.o draw.o -lm

hc:	hc.o draw.o dev.h
	$(CC) $(CFLAGS) -O -o hc hc.o draw.o -lm

ta:	ta.o draw.o dev.h
	$(CC) $(CFLAGS) -O -o ta ta.o draw.o -lm

makedev:	makedev.c dev.h
	$(CC) $(MAKEDEVHACKS) $(CFLAGS) -o makedev makedev.c

dumpaps:	dumpaps.c
	$(CC) $(CFLAGS) dumpaps.c -o dumpaps

src:
	cp makefile $(CFILES) $(HFILES) $R/usr1/src/cmd/troff

driverlist:
	pr d202.c dev.h dva.c dcat.c daps.c makedev.c $R/usr/src/cmd/troff/tc.c

backup:
	cp $R/usr/lib/troff.new $R/usr/lib/otroff.new

clean:
	rm -f titroff *.o a.out make.out core $(DRIVERS) makedev
