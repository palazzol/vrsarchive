###########################################################################
#
#	DESCRIP.MMS - a 'makefile' for VAX/VMS and VAX C V3.0 or greater
#		by Michael Campanella (campanella@cvg.enet.dec.com)
#
# $Header: /home/Vince/cvs/net/starchart/starchart/descrip.mms,v 1.1 1990-03-30 16:38:51 vrs Exp $
#
###########################################################################
#	Makefile for starchart programs
#
#	$Header: /home/Vince/cvs/net/starchart/starchart/descrip.mms,v 1.1 1990-03-30 16:38:51 vrs Exp $
#
#list ONLY the programs you want to use at your site
TARGS= \
	stardsp.exe \
	starX11.exe \
	starXaw.exe \
	starpost.exe \
	startek.exe
#	staruplot
#	starsunv
#	starlaser 
#	starX10 
# startool must be made specially, see below.
# Also consider "postconv.awk"

#SITE DEPENDENCIES
#
# Uncomment out the version appropriate for your site.
# At present dependencies for sysV UNIX
#
#LOCAL=-DSYSV -Dindex=strchr
#LOCAL = /define=(SYSV,"index=strchr","unlink=delete")

# FOR ALL
# define OLD_GREEK if you have the old yale.star file, with a
#                slightly different greek encoding
# To produce programs which allow keyboard user interaction with the -u flag, 
#    see COBJ and starmain.OBJ below.
# FOR X11
# define USE_X_DASHES if your server can draw dashed lines
# define RELEASE3_FONTS if you want to use the X11R3 font names
# define X11R4 if you are using Release 4  (for the athena widgets).
# FOR POSTSCRIPT
# define USE_FINE_MACROS if you want to use finer macros than usual:
#	 star size varies continuously with magnitude.
#        Needs printer with lots of available memory
#
#DEFINES= -DRELEASE3_FONTS -DUSE_X_DASHES -DUSE_FINE_MACROS
#DEFINES= -DRELEASE3_FONTS -DUSE_X_DASHES
#DEFINES = /define=(USE_X_DASHES,RELEASE3_FONTS)

#destination for 'make install', otherwise not important
BINDIR = "/usr/local"

#XINCLUDES is for DECwindows UWS 2.0
#XINCLUDES = -I/usr/include/mit
XINCLUDES =

#list ALL header files
HDRS=icon.h parse_input.h star3.h starXaw.h starXawDlog.h patchlevel.h
#list ALL source files, whether or not you use them
SRCS= interact.c parse_input.c readfile.c starX10.c starX11.c starXaw.c \
	starXawDlog.c starXawHelp.c starXawMwin.c starcust.c \
	stardsp.c starimages.c starlaser.c starm2.c starmain.c \
	starpost.c starsample.c starsunv.c starsupp.c startek.c staruplot.c

#list ALL object files which could be produced
OBJS= interact.OBJ parse_input.OBJ readfile.OBJ starX10.OBJ \
	starX11.OBJ starX11_aw.OBJ starXaw.OBJ starXawDlog.OBJ \
	starXawHelp.OBJ starXawMwin.OBJ starcust.OBJ stardsp.OBJ \
	starimages.OBJ starimages_a.OBJ starlaser.OBJ starm2.OBJ starm2_i.OBJ \
	starmain.OBJ starmain_i.OBJ starpost.OBJ starsunv.OBJ starsupp.OBJ \
	startek.OBJ staruplot.OBJ

ATHENA_OBJS = command.obj form.obj label.obj simple.obj text.obj vpaned.obj -
	asciitext.obj box.obj asciisink.obj strtojust.obj disksrc.obj -
	intrinsic.obj stringsrc.obj utils.obj lower.obj dialog.obj scroll.obj -
	strtoornt.obj graypixmap.obj

STARTOOL=startool.tt startool.icon startool.sh
SUPP=postconv.awk
VMSFILES=decwxtk.opt descrip.mms starchart_init.com vaxcrtl.opt
IBMFILES=pcstar.h Starchar.MSC staribm.c
FILES = Makefile. README. ${SRCS} ${HDRS} ${STARTOOL} ${SUPP} \
	${VMSFILES} ${IBMFILES}

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
FILEROOT=/starchart/
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

.first
	pu/nolog/keep=3
	set prot=(g:rwed,w:re)/default
	define sys sys$library
	define x11 decw$include,sys$disk:[]

.last
	pu/nolog

all : $(TARGS)
	! Done !

#CFLAGS= ${FILEFLAGS} ${LOCAL} ${DEFINES} -g
CFLAGS = -
/list/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS)
#LDFLAGS = -g
LINKFLAGS = /nodebug/trace

#Include interact.OBJ in COBJ to support keyboard user interaction
#COBJ=starmain.OBJ starm2.OBJ starsupp.OBJ readfile.OBJ parse_input.OBJ
COBJ=starmain.OBJ starm2.OBJ starsupp.OBJ readfile.OBJ parse_input.OBJ -
	interact.OBJ 
COBJIM=$(COBJ) starimages.OBJ
COBJIMA=$(COBJ) starimages_a.OBJ

stardsp.exe : $(COBJ) stardsp.OBJ starcust.OBJ
	$ define/user lnk$library sys$library:vaxcrtl.olb
	$ link/exe=$(mms$target) $(LINKFLAGS) $(mms$source_list)
#	$(CC) $(LDFLAGS) ${COBJ} stardsp.OBJ starcust.OBJ -lm -o $@

starlaser.exe : $(COBJIMA) starlaser.OBJ starcust.OBJ
	$(CC) $(LDFLAGS) ${COBJIMA} starlaser.OBJ starcust.OBJ -lm -o $@

starpost.exe : $(COBJ) starpost.OBJ starcust.OBJ
	$ define/user lnk$library sys$library:vaxcrtl.olb
	$ link/exe=$(mms$target) $(LINKFLAGS) $(mms$source_list)
#	$(CC) $(LDFLAGS) $(COBJ) starpost.OBJ starcust.OBJ -lm -o $@

startek.exe :  $(COBJIMA) startek.OBJ starcust.OBJ
	$ define/user lnk$library sys$library:vaxcrtl.olb
	$ link/exe=$(mms$target) $(LINKFLAGS) $(mms$source_list)
#	$(CC) $(LDFLAGS) ${COBJIMA} startek.OBJ starcust.OBJ -lm -o $@

staruplot.exe : $(COBJIMA) staruplot.OBJ starcust.OBJ
	$(CC) $(LDFLAGS) ${COBJIMA} staruplot.OBJ starcust.OBJ -lm -lplot -o $@

starX10.exe : ${COBJIMA} starX10.OBJ starcust.OBJ
	$(CC) $(LDFLAGS) ${COBJIMA} starX10.OBJ starcust.OBJ -lm -lX -o $@

starX11.exe : $(COBJIM) starX11.OBJ starcust.OBJ
	$ define/user lnk$library sys$library:vaxcrtl.olb
	$ link/exe=$(mms$target) $(LINKFLAGS) $(mms$source_list),decwxtk.opt/opt
#	$(CC) $(LDFLAGS) ${COBJIM} starX11.OBJ starcust.OBJ -lm -lX11 -o $@

starXaw.exe : starmain_i.OBJ starm2_i.OBJ starsupp.OBJ readfile.OBJ starX11_aw.OBJ \
		starXaw.OBJ starXawDlog.OBJ starXawHelp.OBJ starXawMwin.OBJ\
		starcust.OBJ starimages.OBJ parse_input.OBJ $(ATHENA_OBJS)
	$ define/user lnk$library sys$library:vaxcrtl.olb
	$ link/exe=$(mms$target) $(LINKFLAGS) $(mms$source_list),decwxtk.opt/opt
#	$(CC) $(LDFLAGS) starmain_i.OBJ starm2_i.OBJ starsupp.OBJ readfile.OBJ \
#		starXaw.OBJ starXawDlog.OBJ starXawHelp.OBJ starXawMwin.OBJ \
#		 starX11_aw.OBJ starcust.OBJ starimages.OBJ parse_input.OBJ\
#		-lm -lXaw -lXmu -lXt -lX11 -o $@

starsunv.exe : starmain_i.OBJ starm2_i.OBJ starsupp.OBJ readfile.OBJ starsunv.OBJ \
		starcust.OBJ starimages.OBJ parse_input.OBJ interact.OBJ
	$(CC) $(LDFLAGS) starmain_i.OBJ starm2_i.OBJ starsupp.OBJ readfile.OBJ \
		starsunv.OBJ starcust.OBJ starimages.OBJ parse_input.OBJ interact.OBJ \
	        -lm -lsuntool -lsunwindow -lpixrect -o $@
startool.exe : starsunv
	echo "You must edit startool, startool.tt and startool.sh,"
	echo "    and install them"
	echo "You must have the program tooltool,"
	echo "    which is available from sun PD archives"
	echo "tooltool -f startool.tt" > startool

# use -DINTERACTIVE_CONTROL in starmain.OBJ and starm2.OBJ
#     to allow keyboard user interaction
starmain.OBJ : starmain.c Makefile star3.h parse_input.h
	$(CC) $(CFLAGS) starmain.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS,INTERACTIVE_CONTROL)
#	$(CC) $(CFLAGS) -DINTERACTIVE_CONTROL -c starmain.c

starm2.OBJ : starm2.c Makefile star3.h
	$(CC) $(CFLAGS) starm2.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS,INTERACTIVE_CONTROL)
#	$(CC) $(CFLAGS) -DINTERACTIVE_CONTROL -c starm2.c

starmain_i.OBJ : starmain.c Makefile star3.h parse_input.h
	- copy starmain.OBJ starmain_n.OBJ
	$(CC) $(CFLAGS) starmain.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS,INTERACTIVE_CONTROL)
#	$(CC) $(CFLAGS) -DINTERACTIVE_CONTROL -c starmain.c
	COPY starmain.OBJ starmain_i.OBJ
	- copy starmain_n.OBJ starmain.OBJ

starm2_i.OBJ : starm2.c Makefile star3.h
	- copy starm2.OBJ starm2_n.OBJ
	$(CC) $(CFLAGS) starm2.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS,INTERACTIVE_CONTROL)
#	$(CC) $(CFLAGS) -DINTERACTIVE_CONTROL -c starm2.c
	COPY starm2.OBJ starm2_i.OBJ
	- copy starm2_n.OBJ starm2.OBJ

readfile.OBJ : readfile.c star3.h
	$(CC) $(CFLAGS) readfile.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS)

starimages.OBJ : starimages.c star3.h
	$(CC) $(CFLAGS) starimages.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS)
#	$(CC) $(CFLAGS) -c starimages.c

#starimages_a.OBJ defines area operations for drivers which otherwise don't
#  support them
starimages_a.OBJ : Makefile starimages.c star3.h
	- copy starimages.OBJ starimages_n.OBJ
	$(CC) $(CFLAGS) starimages.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS,AREAS)
#	$(CC) $(CFLAGS) -DAREAS -c starimages.c
	COPY starimages.OBJ starimages_a.OBJ
	- copy starimages_n.OBJ starimages.OBJ

starX11.OBJ :  starX11.c Makefile icon.h star3.h
	$(CC) $(CFLAGS) $(XINCLUDES) starX11.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS,STARX11)
#	$(CC) $(CFLAGS) $(XINCLUDES) -DSTARX11 -c starX11.c

starX11_aw.OBJ : starX11.c Makefile icon.h star3.h
	- copy starX11.OBJ starX11_n.OBJ
	$(CC) $(CFLAGS) $(XINCLUDES) starX11.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS,STARXAW)
#	$(CC) $(CFLAGS) $(XINCLUDES) -DSTARXAW -c starX11.c
	COPY starX11.OBJ starX11_aw.OBJ
	- copy starX11_n.OBJ starX11.OBJ

starXaw.OBJ : starXaw.c star3.h starXaw.h icon.h
	$(CC) $(CFLAGS) $(XINCLUDES) starXaw.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS)
#	$(CC) $(CFLAGS) $(XINCLUDES) -c starXaw.c

starXawDlog.OBJ : starXawDlog.c star3.h starXaw.h starXawDlog.h
	$(CC) $(CFLAGS) $(XINCLUDES) starXawDlog.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS)
#	$(CC) $(CFLAGS) $(XINCLUDES) -c starXawDlog.c

starXawHelp.OBJ : starXawHelp.c star3.h starXaw.h
	$(CC) $(CFLAGS) $(XINCLUDES) starXawHelp.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS)
#	$(CC) $(CFLAGS) $(XINCLUDES) -c starXawHelp.c

starXawMwin.OBJ : starXawMwin.c star3.h starXaw.h
	$(CC) $(CFLAGS) $(XINCLUDES) starXawMwin.c -
	/define=(SYSV,"index=strchr",USE_X_DASHES,RELEASE3_FONTS)
#	$(CC) $(CFLAGS) $(XINCLUDES) -c starXawMwin.c

starsunv.OBJ : star3.h
interact.OBJ : star3.h parse_input.h
parse_input.OBJ : star3.h parse_input.h
starcust.OBJ : star3.h 
stardsp.OBJ : star3.h 
starlaser.OBJ : star3.h 
starpost.OBJ : star3.h 
starsample.OBJ : star3.h 
starsupp.OBJ : star3.h 
startek.OBJ : star3.h 
staruplot.OBJ : star3.h 
