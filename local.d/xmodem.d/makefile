#
#	$Header: /home/Vince/cvs/local.d/xmodem.d/makefile,v 1.6 2002-11-24 18:12:45 vrs Exp $
#
ALL	= xmodrec$(EXE) xmodtran$(EXE) addcr$(EXE) delcr$(EXE) cr2lf$(EXE) \
	  lf2cr$(EXE)
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

xmodrec$(EXE):	receive.o del.o lf.o
		$(CC) $(CFLAGS) -o $@ del.o lf.o receive.o $(LIBS)

xmodtran$(EXE):	transmit.o add.o cr.o
		$(CC) $(CFLAGS) -o $@ add.o cr.o transmit.o $(LIBS)

addcr$(EXE):	addcr.o add.o
		$(CC) $(CFLAGS) -o $@ add.o addcr.o

delcr$(EXE):	delcr.o del.o
		$(CC) $(CFLAGS) -o $@ del.o delcr.o

lf2cr$(EXE):	lf2cr.o cr.o
		$(CC) $(CFLAGS) -o $@ lf2cr.o cr.o

cr2lf$(EXE):	cr2lf.o lf.o
		$(CC) $(CFLAGS) -o $@ cr2lf.o lf.o
