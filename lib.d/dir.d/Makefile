#
#	$Header: /home/Vince/cvs/lib.d/dir.d/Makefile,v 1.2 1987-02-16 11:19:10 vrs Exp $
#
ALL	= Slibdir.a Mlibdir.a Llibdir.a
OPTIONS	= -O -I.
include ../../CONFIG
OBJS	= closedir.o opendir.o readdir.o seekdir.o telldir.o getwd.o
HFILES	= dir.h
MFILES	= directory.3c getwd.3
SOBJS	= $(OBJS:.o=.S)
MOBJS	= $(OBJS:.o=.M)
LOBJS	= $(OBJS:.o=.L)
#
#	Sure wish I could figure out how to force 'make' to compute this:
#
SLIB	= Slibdir.a(closedir.S) \
	  Slibdir.a(getwd.S) \
	  Slibdir.a(opendir.S) \
	  Slibdir.a(readdir.S) \
	  Slibdir.a(seekdir.S) \
	  Slibdir.a(telldir.S)
MLIB	= Mlibdir.a(closedir.M) \
	  Mlibdir.a(getwd.M) \
	  Mlibdir.a(opendir.M) \
	  Mlibdir.a(readdir.M) \
	  Mlibdir.a(seekdir.M) \
	  Mlibdir.a(telldir.M)
LLIB	= Llibdir.a(closedir.L) \
	  Llibdir.a(getwd.L) \
	  Llibdir.a(opendir.L) \
	  Llibdir.a(readdir.L) \
	  Llibdir.a(seekdir.L) \
	  Llibdir.a(telldir.L)
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

Slibdir.a: $(SLIB)
	ranlib $@

Mlibdir.a: $(MLIB)
	ranlib $@

Llibdir.a: $(LLIB)
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
	cp $(ALL) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(ALL)
	for i in $(ALL); do ranlib $(LIBDIR)/$$i; done
	cp $(HFILES) $(INCDIR)
	cd $(INCDIR); chmod 644 $(HFILES)
	cp $(MFILES) $(MAN3)
	cd $(MAN3); chmod 644 $(MFILES)
