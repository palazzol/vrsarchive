#
#****** Makefile ******************************************************#
#**                                                                  **#
#**                    University of Illinois                        **#
#**                                                                  **#
#**                Department of Computer Science                    **#
#**                                                                  **#
#**   Tool: IFP      			Version: 0.5		     **#
#**                                                                  **#
#**   Author:  Arch D. Robison          Date:   May 1, 1985          **#
#**                                                                  **#
#**   Revised by: Arch D. Robison	Date:   Dec 5, 1986	     **#
#**                                                                  **#
#**   Principal Investigators: Prof. R. H. Campbell                  **#
#**                            Prof. W. J. Kubitz                    **#
#**                                                                  **#
#**                                                                  **#
#**------------------------------------------------------------------**#
#**   (C) Copyright 1987  University of Illinois Board of Trustees   **#
#**                       All Rights Reserved.                       **#
#**********************************************************************#

# makefile for ifp interpreter
#
# This makefile is set up for compiling the IFP interpreter on plain
# vanilla UNIX boxes.  It has been tested on VAXen, Pyramids, and RT/PCs.  
#
# Other machines may required modifications to both this Makefile and
# the header file struct.h.  You should first look at the beginning of
# struct.h, which contains the machine-dependent preprocessor variables.

# for VAXen, Pyramids, RT/PC and other BSD UNIX boxes.
#CFLAGS= -O -DBSD

# Definitions for cross-compiling a MS-DOS version of IFP with a XENIX
# system.  The OPSYS variable in struct.h must also be changed from UNIX
# to MSDOS.
#AOBJS=dos.o
#CFLAGS= -M2l -K -O -dos -F 24000 
#LFLAGS=

# Definitions for compiling a XENIX version of IFP on a PC/AT.
CFLAGS= -M2l -O 
LFLAGS=

# Definitions are for compiling IFP on a CRAY X-MP under CTSS
#CFLAGS = 

# Definitions RT/PC with graphics
#CFLAGS= -DCOMPILE -DGRAPHICS 
#LFLAGS= -DCOMPILE -DGRAPHICS
#GSRC = G_draw.c G_confont.c
#GOBJS=	G_draw.o G_confont.o
#LIBS= -laed

#------------------------------------------------------------------------------

IHDRS=	cache.h inob.h node.h stats.h string.h struct.h umax.h

ISRC=   F_arith.c F_pred.c F_misc.c F_seq.c F_ss.c F_subseq.c F_string.c\
	alloc.c apply.c binio.c cache.c convert.c command.c debug.c\
 	error.c except.c file.c forms.c infun.c inimport.c inob.c list.c\
	main.c node.c outfun.c outob.c stats.c string.c trace.c\
	xdef.c

# 	Miscellaneous source files for special versions of interpreter
VSRC=   dos.s G_draw.c G_confont.c

IOBJS=  F_arith.o F_pred.o F_misc.o F_seq.o F_ss.o F_subseq.o F_string.o \
	alloc.o apply.o binio.o cache.o convert.o command.o debug.o \
	error.o except.o file.o forms.o infun.o inimport.o inob.o list.o \
	main.o node.o outfun.o outob.o stats.o string.o trace.o \
	xdef.o

#-----------------------------------------------------------------------------

ifp:    $(IOBJS) $(AOBJS) $(GOBJS)
	cc $(CFLAGS) $(AOBJS) $(GOBJS) $(IOBJS) $(LIBS) -lm -o ifp
#	strip ifp
	echo "ifp recompiled"

clean:
	rm -f ifp $(IOBJS) $(AOBJS) $(GOBJS)

$(IOBJS):	struct.h

G_confont.o:	G_confont.c			#console vector font
G_draw.o:	struct.h G_draw.c		#optional graphics

F_misc.o:	node.h F_misc.c  		#F_* = primitive functions
F_string.o:	node.h

alloc.o:	node.h umax.h
apply.o:	cache.h node.h stats.h 
cache.o:	cache.h
command.o:	cache.h inob.h node.h stats.h umax.h
error.o:	inob.h
file.o:		inob.h node.h umax.h
forms.o:	node.h stats.h umax.h
infun.o:	inob.h node.h
inimport.o:	inob.h node.h 
inob.o:		inob.h node.h
list.o:		node.h stats.h umax.h 
main.o:		cache.h stats.h umax.h
node.o:		node.h umax.h
outfun.o:	string.h node.h
outob.o:	string.h 
stats.o:	stats.h
string.o:	string.h umax.h
xdef.o:		node.h

#------------------------------------------------------------------------------

lint:      $(IHDRS) $(ISRC) $(GSRC)
	   lint -u -h $(LFLAGS) $(ISRC) $(GSRC) >lint.err

dos:	   ifp
	   doscp -r ifp A:/ifp.exe

