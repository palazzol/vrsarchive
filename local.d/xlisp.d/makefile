ALL	= xlisp$(EXE)
include $(HOME)/CONFIG

OFILES=xlisp.o xlbfun.o xlcont.o xldbug.o xldmem.o xleval.o \
 xlfio.o xlglob.o xlimage.o xlinit.o xlio.o xljump.o xllist.o \
 xlmath.o xlobj.o xlpp.o xlprin.o xlread.o xlstr.o xlstruct.o \
 xlsubr.o xlsym.o xlsys.o xlftab.o msstuff.o

#CFLAGS=-ml -f -O -G -w-pia -w-def -w-aus -Ic:\turboc\include

xlisp$(EXE): $(OFILES)
	$(CC) -o $@ $(OFILES)

clean:
	rm -rf $(OFILES) $(ALL)
#.c.o:
#	tcc -c $(CFLAGS) $<

#xlisp.exe:	$(OFILES) xlftab.o
#	tlink @xlisp.lnk

#xlftab.o:	xlisp.h osdefs.h osptrs.h
#$(OFILES):	xlisp.h

