#	Makefile for observe
# $Header: /home/Vince/cvs/net/starchart/observe/Makefile,v 1.1 1990-03-30 16:37:38 vrs Exp $
#
#
# Copyright (c) 1990 by Craig Counterman. All rights reserved.
#
# This software may be redistributed freely, not sold.
# This copyright notice and disclaimer of warranty must remain
#    unchanged. 
#
# No representation is made about the suitability of this
# software for any purpose.  It is provided "as is" without express or
# implied warranty, to the extent permitted by applicable law.
#
# DISCLAIMER OF WARRANTY
# ----------------------
# The author  disclaims all warranties  with regard to  this software to
# the   extent  permitted  by applicable   law,  including all   implied
# warranties  of merchantability  and  fitness. In  no event shall   the
# author be liable for any special, indirect or consequential damages or
# any  damages whatsoever resulting from  loss of use, data or  profits,
# whether in an action of contract, negligence or other tortious action,
# arising  out of  or in connection with the  use or performance of this
# software.
#
SRCS= astlib.c datelib.c eventcalc.c main.c mooncalc.c objcalc.c \
	orbcalc.c outeph.c outaltaz.c outobs.c outsat.c outsif.c planetcalc.c \
	riseset.c satcalc.c sortlib.c suncalc.c tablecalc.c
HDRS= date.h degree.h observe.h
OBJS= astlib.o datelib.o eventcalc.o main.o mooncalc.o objcalc.o orbcalc.o \
	outeph.o outaltaz.o outobs.o outsat.o outsif.o planetcalc.o \
	riseset.o satcalc.o sortlib.o suncalc.o tablecalc.o 
FILES=Makefile ${SRCS} ${HDRS} README descrip.mms Observe.MSC Astring.h
TARGS=observe


#You may want to -DMAXOBJECTS=500 or fewer.
#CFLAGS=-g -DMAXOBJECTS=500
CFLAGS=-g
LDFLAGS=-lm
#For atari:
#CFLAGS=-VCSD -f  -DDEBUG -DSYSV -DATARI_ST -I.
#CFLAGS=-VCSD -f -DSYSV -DATARI_ST -I.
#CFLAGS= -VCSD -O -VPEEP -f -DSYSV -DATARI_ST -I.
#You may also need to replace {} with ()

DISTDIR=../../dist/observe

all: observe


observe: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o observe -lm

dist:
	cp ${FILES} ${DISTDIR}

clean:
	-/bin/rm ${OBJS} observe

astlib.o: degree.h
datelib.o: date.h 
eventcalc.o: observe.h degree.h
main.o: observe.h date.h
mooncalc.o: observe.h degree.h
objcalc.o: observe.h degree.h
orbcalc.o: observe.h degree.h
outeph.o: observe.h
outaltaz.o: observe.h
outobs.o: observe.h
outsat.o: observe.h
outsif.o: observe.h
planetcalc.o: observe.h degree.h
riseset.o: observe.h degree.h
satcalc.o: observe.h degree.h
suncalc.o: observe.h degree.h
tablecalc.o: observe.h date.h
