#	Makefile for starchart programs
#
#	$Header: /home/Vince/cvs/net/starchart/starchart/Makefile,v 1.1 1990-03-30 16:38:44 vrs Exp $
#
# Read the makefile before making.  There are many things you may wish
# to customize.
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
#list ONLY the programs you want to use at your site
TARGS= \
	stardsp \
	starpost \
	startek \
	staruplot \
	starX11 \
	starXaw \
	starsunv \
	starlaser 
#	starX10 
# startool must be made specially, see below.
# Also consider "postconv.awk"

#SITE DEPENDENCIES
#
# Uncomment out the version appropriate for your site.
# At present dependencies for sysV UNIX
#
#LOCAL=-DSYSV -Dindex=strchr


# FOR ALL
# define OLD_GREEK if you have the old yale.star file, with a
#                slightly different greek encoding
# To produce programs which allow keyboard user interaction with the -u flag, 
#    see COBJ and starmain.o below.
# If you don't want to use the Guide Star Catalog, you can produce
#    slightly smaller executable by defining NO_GSC
# FOR X11
# define USE_X_DASHES if your server can draw dashed lines
# define RELEASE3_FONTS if you want to use the X11R3 font names
# define X11R4 if you are using Release 4  (for the athena widgets).
# FOR POSTSCRIPT
# define USE_FINE_MACROS if you want to use finer macros than usual:
#	 star size varies with 1/10th magnitude increments
#        Needs printer with lots of available memory, but produces
#        smaller postscript files than using the "-a m" option to
#        postscript.
#
#DEFINES= -DRELEASE3_FONTS -DUSE_X_DASHES -DUSE_FINE_MACROS
DEFINES= -DRELEASE3_FONTS -DUSE_X_DASHES

#destination for 'make install', otherwise not important
BINDIR = "/usr/local"

#XINCLUDES is for DECwindows UWS 2.0
XINCLUDES = -I/usr/include/mit
#XINCLUDES =

#list ALL header files
HDRS=icon.h parse_input.h star3.h starXaw.h starXawDlog.h patchlevel.h
#list ALL source files, whether or not you use them
SRCS= interact.c parse_input.c readfile.c starX10.c starX11.c starXaw.c \
	starXawDlog.c starXawHelp.c starXawMwin.c starcust.c \
	stardsp.c starimages.c starlaser.c starm2.c starmain.c \
	starpost.c starsample.c starsunv.c starsupp.c startek.c staruplot.c

#list ALL object files which could be produced
OBJS= interact.o parse_input.o readfile.o starX10.o \
	starX11.o starX11_aw.o starXaw.o starXawDlog.o \
	starXawHelp.o starXawMwin.o starcust.o stardsp.o \
	starimages.o starimages_a.o starlaser.o starm2.o starm2_i.o \
	starmain.o starmain_i.o starpost.o starsunv.o starsupp.o \
	startek.o staruplot.o

STARTOOL=startool.tt startool.icon startool.sh
SUPP=postconv.awk
VMSFILES=decwxtk.opt descrip.mms starchart_init.com vaxcrtl.opt
IBMFILES=pcstar.h Starchar.MSC staribm.c
ATARIFILES=README.st makefile.st starst.c vqgdos.txt vqgdos.s
MACFILES=README.mac
FILES=Makefile README ${SRCS} ${HDRS} ${STARTOOL} ${SUPP} ${VMSFILES} \
	${IBMFILES} ${ATARIFILES} ${MACFILES}

DISTDIR=../../dist/starchart

#The following may be defined here to set default data file locations
# filename	filetype	description
# STARFILE	STARFTYPE	bright star data (yale)
# INDEXFILE	INDEXFTYPE	index to fainter stars (SAO)
# NEBFILE	NEBFTYPE	nebulae
# BOUNDFILE	BOUNDFTYPE	constellation boundaries
# PATTERNFILE	PATTFTYPE	constellation patterns
# CNAMEFILE	CNAMEFTYPE	constellation names
# PLANETFILE	PLANETFTYPE	planet positions

# other files
# CONSTFILE	constellation locations
# RCFILE	resource file

# Define as needed only
# Remember, there are defaults in the code

# Example
FILEROOT=/space/ftp/astro/SAO
STAR="${FILEROOT}/yale.star"
START=LINEREAD
INDEX="${FILEROOT}/index.indx"
INDEXT=INDEXTYPE
# only currently valid index file type
NEB="${FILEROOT}/neb.star"
NEBT=LINEREAD
BOUND="${FILEROOT}/boundaries.star"
BOUNDT=LINEREAD
PATT="${FILEROOT}/pattern.star"
PATTTY=LINEREAD
CNAME="${FILEROOT}/cnames.star"
CNAMET=LINEREAD
PLANET="./planet.star"
# Planets move, so make it local
PLANETTY=LINEREAD
CONS="${FILEROOT}/con.locs"
RC="./.starrc"

FILEFLAGS= \
        -DSTARFILE='$(STAR)' \
        -DSTARFTYPE='$(START)' \
        -DINDEXFILE='$(INDEX)' \
        -DINDEXFTYPE='$(INDEXT)' \
        -DNEBFILE='$(NEB)' \
        -DNEBFTYPE='$(NEBT)' \
        -DBOUNDFILE='$(BOUND)' \
        -DBOUNDFTYPE='$(BOUNDT)' \
        -DPATTERNFILE='$(PATT)' \
        -DPATTFTYPE='$(PATTTY)' \
        -DCNAMEFILE='$(CNAME)' \
        -DCNAMEFTYPE='$(CNAMET)' \
        -DPLANETFILE='$(PLANET)' \
        -DPLANETFTYPE='$(PLANETTY)' \
        -DCONSTFILE='$(CONS)' \
        -DRCFILE='$(RC)'


all: ${TARGS}

CFLAGS= ${FILEFLAGS} ${LOCAL} ${DEFINES} -g
LDFLAGS = -g


#Include interact.o in COBJ to support keyboard user interaction
#COBJ=starmain.o starm2.o starsupp.o readfile.o parse_input.o
COBJ=starmain.o starm2.o starsupp.o readfile.o parse_input.o interact.o 
COBJIM=${COBJ} starimages.o
COBJIMA=${COBJ} starimages_a.o

stardsp: ${COBJ} stardsp.o starcust.o
	$(CC) $(LDFLAGS) ${COBJ} stardsp.o starcust.o -lm -o $@

starlaser: ${COBJIMA} starlaser.o starcust.o
	$(CC) $(LDFLAGS) ${COBJIMA} starlaser.o starcust.o -lm -o $@

starpost: $(COBJ) starpost.o starcust.o
	$(CC) $(LDFLAGS) $(COBJ) starpost.o starcust.o -lm -o $@

startek:  ${COBJIMA} startek.o starcust.o
	$(CC) $(LDFLAGS) ${COBJIMA} startek.o starcust.o -lm -o $@

staruplot: ${COBJIMA} staruplot.o starcust.o
	$(CC) $(LDFLAGS) ${COBJIMA} staruplot.o starcust.o -lm -lplot -o $@

starX10: ${COBJIMA} starX10.o starcust.o
	$(CC) $(LDFLAGS) ${COBJIMA} starX10.o starcust.o -lm -lX -o $@

starX11: ${COBJIM} starX11.o starcust.o
	$(CC) $(LDFLAGS) ${COBJIM} starX11.o starcust.o -lm -lX11 -o $@

starXaw: starmain_i.o starm2_i.o starsupp.o readfile.o starX11_aw.o \
		starXaw.o starXawDlog.o starXawHelp.o starXawMwin.o\
		starcust.o starimages.o parse_input.o
	$(CC) $(LDFLAGS) starmain_i.o starm2_i.o starsupp.o readfile.o \
		starXaw.o starXawDlog.o starXawHelp.o starXawMwin.o \
		 starX11_aw.o starcust.o starimages.o parse_input.o\
		-lm -lXaw -lXmu -lXt -lX11 -o $@

starsunv: starmain_i.o starm2_i.o starsupp.o readfile.o starsunv.o \
		starcust.o starimages.o parse_input.o interact.o
	$(CC) $(LDFLAGS) starmain_i.o starm2_i.o starsupp.o readfile.o \
		starsunv.o starcust.o starimages.o parse_input.o interact.o \
	        -lm -lsuntool -lsunwindow -lpixrect -o $@

startool: starsunv
	echo "You must edit startool, startool.tt and startool.sh,"
	echo "    and install them"
	echo "You must have the program tooltool,"
	echo "    which is available from sun PD archives"
	echo "tooltool -f startool.tt" > startool

# use -DINTERACTIVE_CONTROL in starmain.o and starm2.o
#     to allow keyboard user interaction
starmain.o: starmain.c Makefile star3.h parse_input.h
	$(CC) $(CFLAGS) -DINTERACTIVE_CONTROL -c starmain.c

starm2.o: starm2.c Makefile star3.h
	$(CC) $(CFLAGS) -DINTERACTIVE_CONTROL -c starm2.c



starmain_i.o: starmain.c Makefile star3.h parse_input.h
	-mv starmain.o starmain_n.o
	$(CC) $(CFLAGS) -DINTERACTIVE_CONTROL -c starmain.c
	mv starmain.o starmain_i.o
	-mv starmain_n.o starmain.o

starm2_i.o: starm2.c Makefile star3.h
	-mv starm2.o starm2_n.o
	$(CC) $(CFLAGS) -DINTERACTIVE_CONTROL -c starm2.c
	mv starm2.o starm2_i.o
	-mv starm2_n.o starm2.o

readfile.o: readfile.c star3.h

starimages.o: starimages.c star3.h
	$(CC) $(CFLAGS) -c starimages.c

#starimages_a.o defines area operations for drivers which otherwise don't
#  support them
starimages_a.o: Makefile starimages.c star3.h
	-mv starimages.o starimages_n.o
	$(CC) $(CFLAGS) -DAREAS -c starimages.c
	mv starimages.o starimages_a.o
	-mv starimages_n.o starimages.o

starX11.o: starX11.c Makefile icon.h star3.h
	$(CC) $(CFLAGS) $(XINCLUDES) -DSTARX11 -c starX11.c

starX11_aw.o: starX11.c Makefile icon.h star3.h
	-mv starX11.o starX11_n.o
	$(CC) $(CFLAGS) $(XINCLUDES) -DSTARXAW -c starX11.c
	mv starX11.o starX11_aw.o
	-mv starX11_n.o starX11.o

starXaw.o: starXaw.c star3.h starXaw.h icon.h
	$(CC) $(CFLAGS) $(XINCLUDES) -c starXaw.c

starXawDlog.o: starXawDlog.c star3.h starXaw.h starXawDlog.h
	$(CC) $(CFLAGS) $(XINCLUDES) -c starXawDlog.c

starXawHelp.o: starXawHelp.c star3.h starXaw.h
	$(CC) $(CFLAGS) $(XINCLUDES) -c starXawHelp.c

starXawMwin.o: starXawMwin.c star3.h starXaw.h
	$(CC) $(CFLAGS) $(XINCLUDES) -c starXawMwin.c

starsunv.o: star3.h
interact.o: star3.h parse_input.h patchlevel.h
parse_input.o: star3.h parse_input.h
starcust.o: star3.h 
stardsp.o: star3.h 
starlaser.o: star3.h 
starpost.o: star3.h 
starsample.o: star3.h 
starsupp.o: star3.h 
startek.o: star3.h 
staruplot.o: star3.h 

install: all
	strip $(TARGS)
	mv $(TARGS) $(BINDIR)

dist:
	cp ${FILES} ${DISTDIR}

clean:
	rm -f ${OBJS} ${TARGS} a.out core
