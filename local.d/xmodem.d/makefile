#
#	$Header: /home/Vince/cvs/local.d/xmodem.d/makefile,v 1.4 2002-11-23 07:51:59 Vincent Exp $
#
ALL	= xmodrec xmodtran addcr delcr cr2lf lf2cr
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG
FILES	= receive.c transmit.c addcr.c delcr.c add.c del.c\
          lf2cr.c cr.c lf.c cr2lf.c
OBJS	= receive.o transmit.o addcr.o delcr.o add.o del.o\
          lf2cr.o cr.o lf.o cr2lf.o

install:	$(ALL)
		cp $(ALL) $(BINDIR)
		cd $(BINDIR); chmod 644 $(ALL)

clean:
		rm -f $(OBJS) $(ALL)

xmodrec:	receive.o del.o lf.o
		$(CC) $(CFLAGS) -o $@ del.o lf.o receive.o $(LIBS)

xmodtran:	transmit.o add.o cr.o
		$(CC) $(CFLAGS) -o $@ add.o cr.o transmit.o $(LIBS)

addcr:		addcr.o add.o
		$(CC) $(CFLAGS) -o $@ add.o addcr.o

delcr:		delcr.o del.o
		$(CC) $(CFLAGS) -o $@ del.o delcr.o

lf2cr:		lf2cr.o cr.o
		$(CC) $(CFLAGS) -o $@ lf2cr.o cr.o

cr2lf:		cr2lf.o lf.o
		$(CC) $(CFLAGS) -o $@ cr2lf.o lf.o
