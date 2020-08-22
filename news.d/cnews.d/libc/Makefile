# c news local libc makefile - added by Ian Darwin

INCLUDE = ../include

# delete from SRCS the ones that are already in your C library or -llocal
ALLSRCS = closeall.c efopen.c emalloc.c error.c fgetmfs.c getopt.c \
	memlist.c standard.c warning.c
SRCS = closeall.c emalloc.c fgetmfs.c memlist.c standard.c putenv.c stdfdopen.c

# RANLIB is ranlib on non-USG systems, echo on USG systems
RANLIB=ranlib
RANLIB=echo

CFLAGS = -I$(INCLUDE)

libc.a:	$(SRCS)
	cc $(CFLAGS) -c $?
	ar ru $@ *.o
	rm *.o
	$(RANLIB) $@
