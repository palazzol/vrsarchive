#
# Makefile for UNIX keyed-IO routines.
# 	(c) Ken Wellsch 1985
#
OPTIONS	= -O
MODEL	= -i
include $(HOME)/CONFIG
KLIB	= klib.a(GetBlk.o) \
	  klib.a(GetRec.o) \
	  klib.a(MakBlk.o) \
	  klib.a(MakNam.o) \
	  klib.a(MapBlk.o) \
	  klib.a(MapKey.o) \
	  klib.a(PutBlk.o) \
	  klib.a(PutRec.o) \
	  klib.a(closek.o) \
	  klib.a(creatk.o) \
	  klib.a(dupk.o) \
	  klib.a(kioglob.o) \
	  klib.a(openk.o) \
	  klib.a(readk.o) \
	  klib.a(writek.o)
SRC	= GetBlk.c GetRec.c MakBlk.c MakNam.c \
	  MapBlk.c MapKey.c PutBlk.c PutRec.c \
	  closek.c creatk.c kioglob.c openk.c \
	  dupk.c readk.c writek.c 
OBJ	= $(SRC:.c=.o)
HEADER	= kio.h

.PRECIOUS: klib.a

all:	klib.a

klib.a: $(KLIB)
	$(RANLIB) $@

$(SRC): $(HEADER)
	touch $@

clean:
	rm -f *.o klib.a

backup:
	cp Makefile $(HEADER) $(SRC) bkup

print:
	lpr -Phw -p -J "Keyed I/O Lib" Makefile $(HEADER) $(SRC)

lint:
	lint $(SRC)
