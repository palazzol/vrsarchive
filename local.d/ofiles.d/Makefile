#
#	$Header: /home/Vince/cvs/local.d/ofiles.d/Makefile,v 1.1 1987-02-07 18:15:11 vrs Exp $
#	Ofiles %W%, %G% cspencer
#
# Use -Me for segmented machines only.
CFLAGS	= -O -Me
DESTDIR	= /usr/local
OBJS	= ofiles.o fstab.o

all: ofiles

ofiles:	$(OBJS)
	cc $(OBJS) ${LIBES} -o ofiles

install: ofiles
	mv ofiles ${DESTDIR}
	chown root ${DESTDIR}/ofiles
	chgrp root ${DESTDIR}/ofiles
	chmod 4711 ${DESTDIR}/ofiles

clean:
	rm -f ofiles *.o *.out *.log core
