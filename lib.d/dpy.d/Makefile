#
#	$Header: /home/Vince/cvs/lib.d/dpy.d/Makefile,v 1.5 1987-02-22 20:02:54 vrs Exp $
#
ALL	= Slibdpy.a Mlibdpy.a Llibdpy.a
OPTIONS	= -DUSG
include ../../CONFIG
HFILES	= dpy.h
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
.PRECIOUS:	$(ALL)

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

all:	$(ALL)

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
	rm -f $(SOBJS) $(MOBJS) $(LOBJS) $(ALL) make.out
#
#	Build the works and install it.
#
install: $(ALL) $(LIBDIR) $(INCDIR) $(MAN3)
	cp $(ALL) $(CCLIB)
	cd $(CCLIB); chmod 644 $(ALL)
	for i in $(ALL); do ranlib $(CCLIB)/$$i; done
	cp $(ALL) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(ALL)
	for i in $(ALL); do ranlib $(LIBDIR)/$$i; done
	cp dpy.h $(INCDIR)
	cd $(INCDIR); chmod 644 dpy.h
	cp dpy.3 $(MAN3)
	cd $(MAN3); chmod 644 dpy.3
