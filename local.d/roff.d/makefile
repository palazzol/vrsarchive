#
#	%W% %G%
#
BIN	= /usr/local
CFLAGS	= -O
SHELL	= /bin/sh

roff:	roff.o
	cc $(CFLAGS) -o $@ roff.o

lint:
	lint roff.c | tee lint.out

$(BIN):
	mkdir $@

install: roff
	cp roff $(BIN)
	chmod 711 $(BIN)/roff
	cp roff.1 /usr/man/man1
	chmod 711 /usr/man/man1/roff.1

clean:
	rm -f *.o roff lint.out make.out core
