SHELL	= /bin/sh
FILES	= receive.c transmit.c addcr.c delcr.c add.c del.c\
          lf2cr.c cr.c lf.c cr2lf.c
OBJS	= receive.o transmit.o addcr.o delcr.o add.o del.o\
          lf2cr.o cr.o lf.o cr2lf.o
LIBS 	= -lcurses -ltermlib
CFLAGS	= -O -c -Dindex=strchr
DIR	= /usr/local
ALL	= xmodrec xmodtran addcr delcr cr2lf lf2cr
make:		$(ALL)
clean:
		rm -f $(OBJS) $(ALL)
pr:
		lpr -Plp $(FILES)
receive.o:	receive.c
		cc $(CFLAGS) receive.c
xmodrec:	receive.o del.o lf.o
		cc -O -o $@ del.o lf.o receive.o $(LIBS)
transmit.o:	transmit.c
		cc $(CFLAGS) transmit.c
xmodtran:	transmit.o add.o cr.o
		cc -O -o $@ add.o cr.o transmit.o $(LIBS)
addcr.o:	addcr.c
		cc $(CFLAGS) addcr.c
addcr:		addcr.o add.o
		cc -O -o addcr add.o addcr.o
delcr.o:	delcr.c
		cc $(CFLAGS) delcr.c
delcr:		delcr.o del.o
		cc -O -o delcr del.o delcr.o
add.o:		add.c
		cc $(CFLAGS) add.c
del.o:		del.c
		cc $(CFLAGS) del.c
cr.o:		cr.c
		cc $(CFLAGS) cr.c
lf2cr.o:	lf2cr.c
		cc $(CFLAGS) lf2cr.c
lf2cr:		lf2cr.o cr.o
		cc -O -o lf2cr lf2cr.o cr.o
lf.o:		lf.c
		cc $(CFLAGS) lf.c
cr2lf.o:	cr2lf.c
		cc $(CFLAGS) cr2lf.c
cr2lf:		cr2lf.o lf.o
		cc -O -o cr2lf cr2lf.o lf.o
install:	$(ALL)
		mv xmodrec $(DIR)
		mv xmodtran $(DIR)
		mv addcr $(DIR)
		mv delcr $(DIR)
		mv lf2cr $(DIR)
		mv cr2lf $(DIR)
