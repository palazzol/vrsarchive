# Makefile for a pyramid

# which cursor file we want.
# can be ucl or ukc
CURSOR	= ucl
CFLAGS	= -O -Ipyramid -Dpyramid -DBSD42

basic:  bas1.o bas2.o bas3.o bas4.o bas5.o bas6.o bas7.o bas8.o \
       bas9.o cursor.o termcap.o assist.o term.o
	cc $(CFLAGS) bas1.o bas2.o bas3.o bas4.o bas5.o bas6.o bas7.o \
       bas8.o bas9.o cursor.o termcap.o assist.o term.o -lm -ltermcap -o basic

clean:
	rm -f *.o *.s cursor.c term.c

assist.o: bas.h assist.c
	cc $(CFLAGS) -c assist.c

termcap.o: bas.h termcap.c cursor.c
	cc $(CFLAGS) -c termcap.c

cursor.c: cursor/cursor.c.${CURSOR}
	cp cursor/cursor.c.${CURSOR} cursor.c

cursor.o: cursor.c
	cc $(CFLAGS) -c cursor.c

term.o: term.c
	cc $(CFLAGS) -c term.c

term.c: pyramid/term.c pyramid/conf.h
	cp pyramid/term.c term.c

.c.o:
	cc $(CFLAGS) -c $*.c

bas.h: pyramid/conf.h

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
