# which cursor file we want.
# can be ucl or ukc or ssl
CURSOR = ucl

basic:  bas1.o bas2.o bas3.o bas4.o bas5.o bas6.o bas7.o bas8.o \
       bas9.o cursor.o termcap.o assist.o term.o
	cc -O bas1.o bas2.o bas3.o bas4.o bas5.o bas6.o bas7.o \
       bas8.o bas9.o cursor.o termcap.o assist.o term.o -lm -ltermcap -o basic

clean:
	rm -f *.o *.s cursor.c term.c

assist.o: bas.h assist.c
	cc -O -c -Dm68000 -Uvax -Updp11 assist.c

term.o: term.c
	cc -O -c term.c

term.c: m68000/term.c m68000/conf.h
	cp m68000/term.c term.c

termcap.o: bas.h termcap.c cursor.c
	cc -O -c -Dm68000 -Uvax -Updp11 termcap.c

cursor.c: cursor/cursor.c.${CURSOR}
	cp cursor/cursor.c.${CURSOR} cursor.c

cursor.o: cursor.c
	cc -O -c cursor.c
.c.o:
	cc -O -c -Dm68000 -Uvax -Updp11 $*.c

bas.h:  m68000/conf.h

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
