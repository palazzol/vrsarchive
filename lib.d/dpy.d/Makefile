#
#	@(#)Makefile	1.3 4/8/86
#
SHELL	= /bin/sh
USRLCL	= ..
USRDPY	= $(USRLCL)/dpy
USRLIB	= $(USRDPY)/lib
HFILES	= dpy.h
CFLAGS	= -DUSG
#
#	Use a no-op for $(RANLIB) if you are running on an AT&T UNIX system.
#	UNIX is a trademark of AT&T.  Of course, you'll need to recode the
#	shared data calls.
#
RANLIB	= ranlib
SACRED	= Slibdpy.a Mlibdpy.a Llibdpy.a
#
OBJS	= dpy.o dpymove.o dpyplace.o dpyget.o dpyread.o dpywindow.o
SOBJS	= $(OBJS:.o=.S)
MOBJS	= $(OBJS:.o=.M)
LOBJS	= $(OBJS:.o=.L)
#
#	Sure wish I could figure out how to force 'make' to compute this:
#
SLIB	= Slibdpy.a(dpy.S) \
	  Slibdpy.a(dpymove.S) \
	  Slibdpy.a(dpyplace.S) \
	  Slibdpy.a(dpyget.S) \
	  Slibdpy.a(dpyread.S) \
	  Slibdpy.a(gensubs.S) \
	  Slibdpy.a(dpywindow.S)
MLIB	= Mlibdpy.a(dpy.M) \
	  Mlibdpy.a(dpymove.M) \
	  Mlibdpy.a(dpyplace.M) \
	  Mlibdpy.a(dpyget.M) \
	  Mlibdpy.a(dpyread.M) \
	  Mlibdpy.a(gensubs.M) \
	  Mlibdpy.a(dpywindow.M)
LLIB	= Llibdpy.a(dpy.L) \
	  Llibdpy.a(dpymove.L) \
	  Llibdpy.a(dpyplace.L) \
	  Llibdpy.a(dpyget.L) \
	  Llibdpy.a(dpyread.L) \
	  Llibdpy.a(gensubs.L) \
	  Llibdpy.a(dpywindow.L)
#
#	Define how to make model dependant objects.  Objects with .S extensions
#	are small model, .M are Middle Model, and .L are large model.
#
.SUFFIXES:
.SUFFIXES:	.a .S .M .L .o .s .c
.PRECIOUS:	$(SACRED)

.c.S:
	$(CC) -c $(CFLAGS) -Ms $<
	mv $*.o $@

.c.M:
	$(CC) -c $(CFLAGS) -Mm $<
	mv $*.o $@

.c.L:
	$(CC) -c $(CFLAGS) -Ml $<
	mv $*.o $@

.s.S:
	$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s
	masm -Ml -o$@ tmp.s
	rm tmp.s

.s.M:
	$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s
	masm -Ml -o$@ tmp.s
	rm tmp.s

.s.L:
	$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s
	masm -Ml -o$@ tmp.s
	rm tmp.s

.c.a:
	make $%
	ar uv $@ $%
	rm -f $%

.s.a:
	make $%
	ar uv $@ $%
	rm -f $%

all:	$(SACRED)

Slibdpy.a: $(SLIB)
	ranlib $@

Mlibdpy.a: $(MLIB)
	ranlib $@

Llibdpy.a: $(LLIB)
	ranlib $@

$(SOBJS): $(HFILES)
$(MOBJS): $(HFILES)
$(LOBJS): $(HFILES)

clean:
#	rm -f $(SOBJS) $(MOBJS) $(LOBJS) $(SACRED) make.out

#
#	Build the works and install it.
#
install:	$(SACRED) # $(USRLCL) $(USRDPY) $(USRLIB)
#		cp $(SACRED) $(USRLIB)
#		cd $(USRLIB); chmod 644 $(SACRED)
#		for i in $(SACRED); do ranlib $(USRLIB)/$$i; done

$(USRLCL) $(USRDPY) $(USRLIB):
		mkdir $@
		chmod 755 $@
