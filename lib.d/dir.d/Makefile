#
#	libdir %W% %G%
#
SHELL	= /bin/sh
DIROBJ	= closedir.o opendir.o readdir.o seekdir.o telldir.o
CFLAGS	= -O -I../include

libdir.a:	$(DIROBJ)
		ar cr $@ $?
		ranlib $@

clean:
	rm -f libdir.a *.o make.out core
