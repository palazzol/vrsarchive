#
# Makefile for the Adventure Compiler.
# 	(c) Ken Wellsch 1985
#
OPTIONS	= -O
MODEL	= -i
include $(HOME)/CONFIG
KLIB	= ../kio/klib.a
MAIN	= munge.c
MLIB	= mlib.a(buffers.o) \
	  mlib.a(compile.o) \
	  mlib.a(fio.o) \
	  mlib.a(glob.o) \
	  mlib.a(majors.o) \
	  mlib.a(parse.o) \
	  mlib.a(symtab.o) \
	  mlib.a(util.o)
SRC	= buffers.c compile.c fio.c glob.c \
	  majors.c parse.c symtab.c util.c
OBJ	= $(SRC:.c=.o)
HEADER	= mdefs.h

.PRECIOUS: mlib.a

all:	munge$(EXE)

munge$(EXE): $(MAIN) mlib.a
	$(CC) $(CFLAGS) $(MAIN) mlib.a $(KLIB) $(LIBS) -o munge$(EXE)
	size munge$(EXE)

$(SRC) $(MAIN):: mdefs.h
	touch $@

mlib.a:	$(OBJ) $(MLIB)
	$(RANLIB) $@

clean:
	rm -f *.o mlib.a munge$(EXE)

backup:
	cp Makefile $(HEADER) $(MAIN) $(SRC) bkup

print:
	lpr -Phw -p -J "Munge" Makefile $(HEADER) $(MAIN) $(SRC)

lint:
	lint $(SRC) $(MAIN)
