#	Makefile for dataconv programs
#
#	$Header: /home/Vince/cvs/net/starchart/dataconv/Makefile,v 1.1 1990-03-30 16:37:14 vrs Exp $
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
# 
#
#

SRCS=dataconv.c tosize.c sif_to_text.c
HDRS=
OBJS=dataconv.o tosize.o sif_to_text.o
FILES=Makefile ${SRCS} ${HDRS} Dataconv.MSC README tosize.perl
TARGS=dataconv tosize sif_to_text

CFLAGS=-O
LDFLAGS=-lm


DISTDIR=../../dist/dataconv

all: dataconv tosize sif_to_text


dataconv: dataconv.o
	${CC} ${CFLAGS} -o $@ dataconv.o ${LDFLAGS}

tosize: tosize.o
	${CC} ${CFLAGS} -o $@ tosize.o ${LDFLAGS}

sif_to_text: sif_to_text.o
	${CC} ${CFLAGS} -o $@ sif_to_text.o ${LDFLAGS}

dist:
	cp ${FILES} ${DISTDIR}

clean:
	rm -f ${OBJS} ${TARGS} a.out core
