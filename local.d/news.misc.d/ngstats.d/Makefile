#
#	$Header: /home/Vince/cvs/local.d/news.misc.d/ngstats.d/Makefile,v 1.5 2002-11-23 18:58:20 Vincent Exp $
#
ALL	= ngstats
MODEL	= -i
OPTIONS	= -O -I. -DSPOOLDIR=\"$(SPOOLN)\" -DACTIVE=\"$(LIBNEWS)/active\"
include $(HOME)/CONFIG
DIROBJ	= closedir.o opendir.o readdir.o seekdir.o telldir.o
OBJS	= ngstats.o sysname.o

ngstats:	$(OBJS) libdir.a
		cc -o $@ $(CFLAGS) $(OBJS) libdir.a

libdir.a:	$(DIROBJ)
		ar cr $@ $?
		ranlib $@

clean:
		rm -f $(ALL) $(OBJS) $(DIROBJ) libdir.a
