#
#       Makefile for MSDOS tools
#
MANS	= mdir.1 mread.1 mwrite.1 mdel.1 mtype.1 mmd.1 mrd.1 mren.1
ALL	= $(MANS:.1=)
OPTIONS = -O
include ../../CONFIG
LINT = lint
SHAR = shar -a

MDIR = mdir.o getfat.o init.o search.o match.o convdate.o unixname.o subdir.o \
	isdir.o
MREAD = mread.o getfat.o init.o search.o match.o unixname.o subdir.o
MWRITE = mwrite.o getfat.o init.o search.o fixname.o unixname.o putfat.o \
	subdir.o mkentry.o isdir.o
MDEL = mdel.o getfat.o init.o search.o match.o unixname.o putfat.o subdir.o
MTYPE = mtype.o getfat.o init.o search.o match.o unixname.o subdir.o
MMD = mmd.o getfat.o init.o search.o fixname.o unixname.o putfat.o subdir.o \
	mkentry.o
MRD = mrd.o getfat.o init.o search.o unixname.o putfat.o subdir.o
MREN = mren.o getfat.o init.o search.o fixname.o unixname.o putfat.o subdir.o \
	isdir.o

mdir:	$(MDIR)
	$(CC) $(CFLAGS) $(MDIR) -o $@

mread:	$(MREAD)
	$(CC) $(CFLAGS) $(MREAD) -o mread

mwrite:	$(MWRITE)
	$(CC) $(CFLAGS) $(MWRITE) -o mwrite

mdel:	$(MDEL)
	$(CC) $(CFLAGS) $(MDEL) -o mdel

mtype:	$(MTYPE)
	$(CC) $(CFLAGS) $(MTYPE) -o mtype

mmd:	$(MMD)
	$(CC) $(CFLAGS) $(MMD) -o mmd

mrd:	$(MRD)
	$(CC) $(CFLAGS) $(MRD) -o mrd

mren:	$(MREN)
	$(CC) $(CFLAGS) $(MREN) -o mren

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp Mdir.1 $(MAN1)/mdir.1
	cp Mread.1 $(MAN1)/mread.1
	cp Mwrite.1 $(MAN1)/mwrite.1
	cp Mdel.1 $(MAN1)/mdel.1
	cp Mtype.1 $(MAN1)/mtype.1
	cp Mmd.1 $(MAN1)/mmd.1
	cp Mrd.1 $(MAN1)/mrd.1
	cp Mren.1 $(MAN1)/mren.1
	cd $(BINDIR); chmod 711 $(MANS)

clean:
	rm -f convdate.o fixname.o getfat.o init.o isdir.o match.o mdel.o
	rm -f mdir.o mkentry.o mmd.o mrd.o mread.o mren.o mtype.o mwrite.o
	rm -f putfat.o search.o subdir.o unixname.o $(ALL)

lint:
	$(LINT) mdir.c getfat.c init.c search.c match.c convdate.c subdir.c \
	unixname.c isdir.c
	$(LINT) mread.c getfat.c init.c search.c match.c unixname.c subdir.c
	$(LINT) mwrite.c getfat.c init.c search.c fixname.c unixname.c \
	putfat.c subdir.c mkentry.c isdir.c
	$(LINT) mdel.c getfat.c init.c search.c match.c unixname.c putfat.c \
	subdir.c
	$(LINT) mtype.c getfat.c init.c search.c match.c unixname.c subdir.c
	$(LINT) mmd.c getfat.c init.c search.c fixname.c unixname.c putfat.c \
	subdir.c mkentry.c
	$(LINT) mrd.c getfat.c init.c search.c unixname.c putfat.c subdir.c
	$(LINT) mren.c getfat.c init.c search.c fixname.c unixname.c putfat.c \
	subdir.c

shar:
	$(SHAR) Readme Makefile Mdel.1 Mdir.1 Mmd.1 Mrd.1 Mread.1 Mren.1 \
	Mtype.1 Mwrite.1 msdos.h convdate.c fixname.c getfat.c init.c \
	isdir.c match.c mdel.c mdir.c > mtools1
	$(SHAR) mkentry.c mmd.c mrd.c mread.c mren.c mtype.c mwrite.c \
	putfat.c search.c subdir.c unixname.c > mtools2

getfat.o:	msdos.h
init.o:		msdos.h
isdir.o:	msdos.h
mdel.o:		msdos.h
mdir.o:		msdos.h
mkentry.o:	msdos.h
mmd.o:		msdos.h
mrd.o:		msdos.h
mread.o:	msdos.h
mren.o:		msdos.h
mtype.o:	msdos.h
mwrite.o:	msdos.h
putfat.o:	msdos.h
search.o:	msdos.h
subdir.o:	msdos.h
