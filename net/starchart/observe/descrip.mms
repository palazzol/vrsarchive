###########################################################################
#
#	VMS 'makefile' for OBSERVE.  Type MMS to use.
#		by Michael Campanella (campanella@cvg.enet.dec.com)
#
###########################################################################
#	Makefile for observe
# $Header: /home/Vince/cvs/net/starchart/observe/descrip.mms,v 1.1 1990-03-30 16:37:47 vrs Exp $
#
SRCS= astlib.c datelib.c eventcalc.c main.c mooncalc.c objcalc.c \
	orbcalc.c outeph.c outaltaz.c outobs.c outsat.c outsif.c planetcalc.c \
	riseset.c satcalc.c sortlib.c suncalc.c tablecalc.c
HDRS= date.h degree.h observe.h
OBJS= astlib.OBJ datelib.OBJ eventcalc.OBJ main.OBJ mooncalc.OBJ objcalc.OBJ orbcalc.OBJ \
	outeph.OBJ outaltaz.OBJ outobs.OBJ outsat.OBJ outsif.OBJ planetcalc.OBJ riseset.OBJ satcalc.OBJ \
	sortlib.OBJ suncalc.OBJ tablecalc.OBJ 
FILES=Makefile ${SRCS} ${HDRS} descrip.mms Observe.MSC
TARGS=observe.exe

#CFLAGS=-g
CFLAGS = /list/define=(SYSV,"index=strchr")
LDFLAGS=-lm

DISTDIR=../../dist/observe

.first
	pu/keep=3/nolog
	define sys sys$library

.last
	pu/nolog

all : observe.exe
	! Done !

observe.exe : $(OBJS)
	$ define/user lnk$library sys$library:vaxcrtl.olb
	$ link/trace/exe=$(mms$target) $(mms$source_list)
#	${CC} ${CFLAGS} ${OBJS} -o observe -lm

astlib.OBJ : degree.h
datelib.OBJ : date.h 
eventcalc.OBJ : observe.h degree.h
main.OBJ : observe.h date.h
mooncalc.OBJ : observe.h degree.h
objcalc.OBJ : observe.h degree.h
orbcalc.OBJ : observe.h degree.h
outeph.OBJ : observe.h
outobs.OBJ : observe.h
outsat.OBJ : observe.h
outsif.OBJ : observe.h
planetcalc.OBJ : observe.h degree.h
riseset.OBJ : observe.h degree.h
satcalc.OBJ : observe.h degree.h
suncalc.OBJ : observe.h degree.h
tablecalc.OBJ : observe.h date.h
