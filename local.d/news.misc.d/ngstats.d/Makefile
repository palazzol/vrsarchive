#
#	ngstats %W% %G%
#
SHELL	= /bin/sh
DIROBJ	= closedir.o opendir.o readdir.o seekdir.o telldir.o
CFLAGS	= -O -I. -i -Dindex=strchr

ngstats:	ngstats.o sysname.o libdir.a
		cc -o $@ $(CFLAGS) ngstats.o sysname.o libdir.a

libdir.a:	$(DIROBJ)
		ar cr $@ $?
		ranlib $@

clean:
	rm -f ngstats libdir.a *.o make.out core
