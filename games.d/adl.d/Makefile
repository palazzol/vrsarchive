#GAMDIR	= /usr/games
#GAMLIB	= $(GAMDIR)/lib
#LIBS	= -lcurses -ltermcap
include /etc/CONFIG
BIN	= $(GAMLIB)/adl
DIST	= ../adl.dist/SHAR
# AR	= shar -c -v -d
AR	= post -c -v -p 1 17

all install clean :
		cd util;	make $@
		cd adlcomp;	make $@
		cd adlrun;	make $@
		cd samples;	make $@
		cd include;	make $@
		cd misc;	make $@
		rm -f adl.shar

post	:	shar
		mv adl.shar ${DIST}/adl_01.shar
		mv adlcomp/adlcomp1.shar ${DIST}/adl_02.shar
		mv adlcomp/adlcomp2.shar ${DIST}/adl_03.shar
		mv adlrun/adlrun1.shar ${DIST}/adl_04.shar
		mv adlrun/adlrun2.shar ${DIST}/adl_05.shar
		mv include/include.shar ${DIST}/adl_06.shar
		mv misc/misc.shar ${DIST}/adl_07.shar
		mv samples/samples.shar ${DIST}/adl_08.shar
		mv util/util.shar ${DIST}/adl_09.shar
		mv samples/aard/aard1.shar ${DIST}/adl_10.shar
		mv samples/aard/aard2.shar ${DIST}/adl_11.shar
		mv samples/demos/demos.shar ${DIST}/adl_12.shar
		mv samples/mpu/mpu1.shar ${DIST}/adl_13.shar
		mv samples/mpu/mpu2.shar ${DIST}/adl_14.shar

shar	:	adl.shar
		(cd util;	make shar);
		(cd adlcomp;	make shar);
		(cd adlrun;	make shar);
		(cd samples;	make shar);
		(cd include;	make shar);
		(cd misc;	make shar);

adl.shar	: README	COPYRIGHT	PORTING		\
		  Makefile	man		adlcomp		\
		  adlrun	misc		util		\
		  include	samples		samples/aard	\
		  samples/mpu	samples/demos	man/*.6
		${AR}	README		COPYRIGHT	PORTING		\
			Makefile	man		adlcomp		\
			adlrun		misc		util		\
			include		samples		samples/aard	\
			samples/mpu	samples/demos	man/*.6		\
		> adl.shar

touch	:
		(cd util;	make -t);
		(cd adlcomp;	make -t);
		(cd adlrun;	make -t);
		(cd samples;	make touch);
		(cd misc;	make -t);
