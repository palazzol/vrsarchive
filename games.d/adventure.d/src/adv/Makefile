#
# Makefile for the Adventure interpreter.
# 	(c) Ken Wellsch 1985
#
OPTIONS	= -O -DADV=\"$(GAMLIB)/adv\"
MODEL	= -i
include ../../../../CONFIG
KLIB	= ../kio/klib.a
ALIB	= alib.a(advglob.o) \
	  alib.a(cache.o) \
	  alib.a(command.o) \
	  alib.a(condition.o) \
	  alib.a(error.o) \
	  alib.a(exec.o) \
	  alib.a(process.o) \
	  alib.a(savgm.o) \
	  alib.a(say.o) \
	  alib.a(showop.o) \
	  alib.a(symtab.o) \
	  alib.a(util.o) \
	  alib.a(values.o) \
	  alib.a(webster.o)
MAIN	= adv.c
SRC	= error.c advglob.c cache.c command.c condition.c \
	exec.c process.c savgm.c say.c showop.c symtab.c \
	util.c values.c webster.c
OBJ	= $(SRC:.c=.o)
HEADER	= adefs.h

.PRECIOUS: alib.a

all:	adv

adv:	alib.a
	$(CC) $(CFLAGS) $(MAIN) alib.a $(KLIB) $(LIBS) -o $@
	size adv

$(SRC):: $(HEADER)
	touch $@

alib.a:	$(ALIB)
	$(RANLIB) $@

clean:
	rm -f *.o adv alib.a

backup:
	cp Makefile $(HEADER) $(SRC) $(MAIN) bkup

lint:
	lint $(SRC) $(MAIN)

print:
	lpr -Phw -p -J "ADV" Makefile $(HEADER) $(MAIN) $(SRC)
