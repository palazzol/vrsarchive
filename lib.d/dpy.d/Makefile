#
#	$Header: /home/Vince/cvs/lib.d/dpy.d/Makefile,v 1.8 2002-11-24 03:08:50 Vincent Exp $
#
ALL	= Slibdpy.a Mlibdpy.a Llibdpy.a libdpy.a
OPTIONS	= -DUSG
include $(HOME)/CONFIG
HFILES	= dpy.h
#
OBJS	= dpy.o dpymove.o dpyplace.o dpyget.o dpyread.o gensubs.o dpywindow.o
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
	if i286; then \
		$(CC) -c $(CFLAGS) -Ms $<; \
		mv $*.o $@; \
	else \
		$(CC) -c $(CFLAGS) $<; \
		mv $*.o $@; \
	fi

.c.M:
	if i286; then \
		$(CC) -c $(CFLAGS) -Mm $<; \
		mv $*.o $@; \
	else \
		$(CC) -c $(CFLAGS) $<; \
		mv $*.o $@; \
	fi

.c.L:
	if i286; then \
		$(CC) -c $(CFLAGS) -Ml $<; \
		mv $*.o $@; \
	else \
		$(CC) -c $(CFLAGS) $<; \
		mv $*.o $@; \
	fi

.s.S:
	if i286; then \
		$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s; \
		masm -Ml -o$@ tmp.s; \
		rm tmp.s; \
	else \
		$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s; \
		masm -o$@ tmp.s; \
		rm tmp.s; \
	fi

.s.M:
	if i286; then \
		$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s; \
		masm -Ml -o$@ tmp.s; \
		rm tmp.s; \
	else \
		$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s; \
		masm -o$@ tmp.s; \
		rm tmp.s; \
	fi

.s.L:
	if i286; then \
		$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s; \
		masm -Ml -o$@ tmp.s; \
		rm tmp.s; \
	else \
		$(CC) $(CFLAGS) -Ms -E $*.s | sed '/^#/d' >tmp.s; \
		masm -o$@ tmp.s; \
		rm tmp.s; \
	fi

.c.a:
	make $%
	ar ruv $@ $%
	rm -f $%

.s.a:
	make $%
	ar ruv $@ $%
	rm -f $%

all:	$(ALL)

libdpy.a Slibdpy.a: $(SLIB)
	rm -f libdpy.a; ln Slibdpy.a libdpy.a
	$(RANLIB) $@

Mlibdpy.a: $(MLIB)
	$(RANLIB) $@

Llibdpy.a: $(LLIB)
	$(RANLIB) $@

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
	for i in $(ALL); do $(RANLIB) $(CCLIB)/$$i; done
	cp $(ALL) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(ALL)
	for i in $(ALL); do $(RANLIB) $(LIBDIR)/$$i; done
	cp dpy.h $(INCDIR)
	cd $(INCDIR); chmod 644 dpy.h
	cp dpy.3 $(MAN3)
	cd $(MAN3); chmod 644 dpy.3
