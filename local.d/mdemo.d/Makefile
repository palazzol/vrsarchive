#
#	@(#)Makefile	1.3	3/16/85
#
BIN	= /usr/local
CFLAGS	= -M2e
SHELL	= /bin/sh
LFLAGS	= -lcurses -ltermcap # -lx

mdemo:		mdemo.c
		cc $(CFLAGS) -o $@ mdemo.c $(LFLAGS)

install:	mdemo
		cp mdemo $(BIN)
		chown root $(BIN)/mdemo
		chgrp root $(BIN)/mdemo
		chmod 4711 $(BIN)/mdemo

clean:
		rm -f mdemo mdemo.o fluff

lint:
		lint $(LFLAGS) mdemo.c | tee fluff
