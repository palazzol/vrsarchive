# Makefile for xenix 286

# which cursor file we want.
# can be ucl or ukc
CURSOR = ucl
CFLAGS	= -O -Ix286 -M2m
OBJS	= bas1.o bas2.o bas3.o bas4.o bas5.o bas6.o bas7.o bas8.o \
	  bas9.o cursor.o termcap.o assist.o term.o

basic:	$(OBJS)
	cc $(CFLAGS) $(OBJS) -lm -ltermcap -o $@

clean:
	rm -f *.o *.s cursor.c term.c

cursor.c: cursor/cursor.c.${CURSOR}
	cp cursor/cursor.c.${CURSOR} cursor.c

term.c: x286/term.c x286/conf.h
	cp x286/term.c term.c

bas.h: x286/conf.h

assist.o: bas.h assist.c
bas1.o: bas1.c bas.h
bas2.o: bas2.c bas.h
bas3.o: bas3.c bas.h
bas4.o: bas4.c bas.h
bas5.o: bas5.c bas.h
bas6.o: bas6.c bas.h
bas7.o: bas7.c bas.h
bas7.c: cursor.c
bas8.o: bas8.c bas.h
bas9.o: bas9.c bas.h
cursor.o: cursor.c
term.o: term.c
termcap.o: bas.h termcap.c cursor.c
