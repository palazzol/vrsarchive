ALL		= adldebug$(EXE) adltouch$(EXE)
MODEL	= -M2l
OPTIONS	= -O -DHPUX -I${INC}
include $(HOME)/CONFIG
# Where utility routines live
UTL = ../util

# Where include files live
INC = ../include

# Commands for shar
# AR = shar -c -v
AR = post -c -v -p 7 17
B = ..
D = misc/

# Where adldebug and adltouch will be installed
#BIN = /users/cunniff/bin
BIN		= ${GAMLIB}

# The utilities used by adldebug
UTIL =	${UTL}/virtmem.o	${UTL}/vstring.o	${UTL}/mytime.o

# The rules
install		:	adldebug$(EXE) adltouch$(EXE)
	strip adldebug$(EXE) adltouch$(EXE)
	cp adldebug$(EXE) adltouch$(EXE) ${BIN}
	chmod 755 ${BIN}/adldebug$(EXE) ${BIN}/adltouch$(EXE)

shar		:	misc.shar

clean		:
	rm -f misc.shar adldebug.o adltouch.o $(ALL)

misc.shar	:	README Makefile *.c
	( cd ${B} ; ${AR}	\
		${D}README	${D}COPYRIGHT	${D}Makefile	${D}*.c \
	> ${D}misc.shar )

adldebug$(EXE)	:	adldebug.o	${UTIL}
	$(CC) $(CFLAGS) -o $@ adldebug.o ${UTIL}

adldebug.o	:	adldebug.c \
			${INC}/adltypes.h	${INC}/adlprog.h \
			${INC}/adldef.h		${INC}/vstring.h \
			${INC}/virtmem.h

adltouch$(EXE)	:	adltouch.o
	$(CC) $(CFLAGS) -o $@ adltouch.o

adltouch.o	:	adltouch.c
