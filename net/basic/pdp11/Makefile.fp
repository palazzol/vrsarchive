# which cursor control file you want . either ucl or ukc
CURSOR	= ucl
CFLAGS	= -i -O -Ipdp11

basic:  bas1.o bas2.o bas3.o bas4.o bas5.o bas6.o bas7.o bas8.o \
       bas9.o cursor.o termcap.o fpassist.o term.o
	cc -s $(CFLAGS) fpassist.o bas1.o bas2.o bas3.o bas4.o bas5.o \
       bas6.o bas7.o bas8.o bas9.o cursor.o termcap.o term.o -lm -o basic

clean:
	rm -f *.o *.s term.c cursor.c

fpassist.o: pdp11/fpassist.s
	cp pdp11/fpassist.s fpassist.s
	cc $(CFLAGS) -c fpassist.s
	rm -f fpassist.s

term.o: term.c
	cc $(CFLAGS) -c term.c

term.c: pdp11/term.c pdp11/conf.h
	cp pdp11/term.c term.c


cursor.c: cursor/cursor.c.${CURSOR}
	cp cursor/cursor.c.${CURSOR} cursor.c

cursor.o: cursor.c
	cc $(CFLAGS) -c cursor.c

termcap.o: bas.h termcap.c
	cc $(CFLAGS) -c termcap.c

.c.o:   $*.c
	cc $(CFLAGS) -f -c $*.c

bas.h: pdp11/conf.h

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
