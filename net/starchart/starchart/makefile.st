#	Makefile for atari ST
#
#	$Header: /home/Vince/cvs/net/starchart/starchart/makefile.st,v 1.1 1990-03-30 16:38:54 vrs Exp $
#
TARGS= starst

#SITE DEPENDENCIES
#
LOCAL=-DSYSV -DATARI_ST -Ic:\include\local -DREADMODE=\"rb\"


# FOR ALL
# define OLD_GREEK if you have the old yale.star file, with a
#                slightly different greek encoding
# To produce programs which allow keyboard user interaction with the -u flag, 
#    see COBJ and starmain.o below.
# FOR POSTSCRIPT
# define USE_FINE_MACROS if you want to use finer macros than usual:
#	 star size varies continuously with magnitude.
#        Needs printer with lots of available memory
#
#DEFINES= -DRELEASE3_FONTS -DUSE_X_DASHES -DUSE_FINE_MACROS
DEFINES= -DRELEASE3_FONTS -DUSE_X_DASHES

#destination for 'make install', otherwise not important
BINDIR = "g:\starch32"

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
FILEROOT=g:\\stardata
STAR="$(FILEROOT)\\yale.bin"
START=BINFULL
INDEX="$(FILEROOT)\\index.ind"
INDEXT=INDEXTYPE
# only currently valid index file type
NEB="$(FILEROOT)\\deepsky.bin"
NEBT=BINFULL
BOUND="$(FILEROOT)\\boundaries.str"
BOUNDT=LINEREAD
PATT="$(FILEROOT)\\pattern.str"
PATTTY=LINEREAD
CNAME="$(FILEROOT)\\cnames.str"
CNAMET=LINEREAD
PLANET=".\\planet.str"
# Planets move, so make it local
PLANETTY=LINEREAD
CONS="$(FILEROOT)\\con.loc"
RC=".\\star.rc"

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


all: $(TARGS)

#CFLAGS= $(FILEFLAGS) $(LOCAL) $(DEFINES) -f68881
#LDFLAGS = 
CFLAGS= $(FILEFLAGS) $(LOCAL) $(DEFINES) -VCSD -f
LDFLAGS = -VCSD -f


#Include interact.o in COBJ to support keyboard user interaction
#COBJ=starmain.o starm2.o starsupp.o readfile.o parse_input.o
COBJ=starmain.o starm2.o starsupp.o readfile.o parse_input.o interact.o 
COBJIM=$(COBJ) starimages.o
COBJIMA=$(COBJ) starimages_a.o

starst: $(COBJ) starst.o starcust.o vqgdos.o
	$(CC) $(LDFLAGS) $(COBJ) starst.o starcust.o vqgdos.o -lm -laes -lvdi\
	                                                 -o $@
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

interact.o: star3.h parse_input.h patchlevel.h
parse_input.o: star3.h parse_input.h
starcust.o: star3.h 
starsample.o: star3.h 
starsupp.o: star3.h 
starst.o: star3.h


install: all
	strip $(TARGS)
	mv $(TARGS) $(BINDIR)

dist:
	cp $(FILES) $(DISTDIR)

clean:
	rm -f $(OBJS) $(TARGS) a.out core
