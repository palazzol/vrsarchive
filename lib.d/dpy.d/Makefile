# @(#)makefile.usg	1.5	2/27/85
# Makefile for USG systems

.SUFFIXES: .c .o .s

CC	= cc
OFLAG	= -O
DFLAG	=
SYSTEM	= -DUSG -Ml
CFLAGS	= $(OFLAG) $(DFLAG) $(SYSTEM)
AS	= as
LIBDIR	= /lib
LIB	= libdpy.a
SHELL	= /bin/sh
CFILES	= dpy.c dpymove.c dpyplace.c dpyget.c dpyread.c dpywindow.c
OFILES	= dpy.o dpymove.o dpyplace.o dpyget.o dpyread.o dpywindow.o

SOURCES = dpy.h ${CFILES} vaxsubs.s ns32ksubs.s gensubs.c

# Machine dependent assembly routines.  Define MACHINEFILE appropriately
# to select the target machine.  Gensubs is a portable version of the
# subroutines applicable to any machine (but slower than the assembly ones).

GENFILE		= gensubs.o	# generic subroutines
NS32KFILE	= ns32ksubs.o	# National Semiconductor 32032 subroutines
VAXFILE		= vaxsubs.o	# VAX subroutines
MACHINEFILE	= ${GENFILE}	# subroutines to be used


LIBOBJS =	$(LIB)(dpy.o) \
		$(LIB)(dpymove.o) \
		$(LIB)(dpyplace.o) \
		$(LIB)(dpyget.o) \
		$(LIB)(dpyread.o) \
		$(LIB)(dpywindow.o) \
		$(LIB)(${MACHINEFILE})

libdpy.a: ${LIBOBJS}
	ranlib libdpy.a

sources: ${SOURCES}

$(LIBOBJS) ${OFILES}: dpy.h

install: libdpy.a
	cp libdpy.a ${LIBDIR}
	chmod 644 ${LIBDIR}/libdpy.a

test1:	test1.o libdpy.a
	$(CC) test1.o $(LDFLAGS) -o test1 libdpy.a -lcurses -ltermcap

test2:	test2.o libdpy.a
	$(CC) test2.o $(LDFLAGS) -o test2 libdpy.a -lcurses -ltermcap

print:	${SOURCES} makefile
	pr -w120 -n ${SOURCES} makefile >dpy.lst
	@echo dpy.lst finished

clean:
	rm -f *.o libdpy.a test1 test2

PRECIOUS:	libdpy.a

.s.o:
	${AS} -o $*.o $*.s

.c.o:
	${CC} -c ${CFLAGS} $*.c

.s~.s:
	$(GET) $(GFLAGS) -p $< > $*.s
