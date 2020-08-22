#
#	$Header: /home/Vince/cvs/lib.d/fft.d/Makefile,v 1.4 2002-11-23 07:46:09 Vincent Exp $
#
ALL	= Slibfft.a Mlibfft.a Llibfft.a
OPTIONS	= -O -I.
include $(HOME)/CONFIG
OBJS	= fft2.o
SOBJS	= $(OBJS:.o=.S)
MOBJS	= $(OBJS:.o=.M)
LOBJS	= $(OBJS:.o=.L)
#
#	Sure wish I could figure out how to force 'make' to compute this:
#
SLIB	= Slibfft.a(fft2.S)
MLIB	= Mlibfft.a(fft2.M)
LLIB	= Llibfft.a(fft2.L)
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

Slibfft.a: $(SLIB)
	ranlib $@

Mlibfft.a: $(MLIB)
	ranlib $@

Llibfft.a: $(LLIB)
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
	cp fft2.3 $(MAN3)
	cd $(MAN3); chmod 644 fft2.3
