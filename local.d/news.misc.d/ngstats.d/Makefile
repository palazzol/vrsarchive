#
#	$Header: /home/Vince/cvs/local.d/news.misc.d/ngstats.d/Makefile,v 1.2 1987-02-15 16:26:18 vrs Exp $
#
ALL	= ngstats
MODEL	= -i
OPTIONS	= -O -I. -DSPOOLDIR=\"$(SPOOLN)\" -DACTIVE=\"$(LIBNEWS)/active\"
include ../../../CONFIG
DIROBJ	= closedir.o opendir.o readdir.o seekdir.o telldir.o

ngstats:	ngstats.o sysname.o libdir.a
		cc -o $@ $(CFLAGS) ngstats.o sysname.o libdir.a

libdir.a:	$(DIROBJ)
		ar cr $@ $?
		ranlib $@

clean:
		rm -f $(ALL) $(DIROBJ) libdir.a
