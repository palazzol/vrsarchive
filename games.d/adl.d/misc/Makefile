ALL		= adldebug adltouch
MODEL	= -M2l
OPTIONS	= -O -DHPUX -I${INC}
include /etc/CONFIG
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

# The utilities used by adldebug
UTIL =	${UTL}/virtmem.o	${UTL}/vstring.o	${UTL}/mytime.o

# The rules
install		:	adldebug adltouch
	strip adldebug adltouch
	cp adldebug adltouch ${BIN}
	chmod 755 ${BIN}/adldebug ${BIN}/adltouch

shar		:	misc.shar

clean		:
	rm -f misc.shar adldebug.o adltouch.o adldebug adltouch

misc.shar	:	README Makefile *.c
	( cd ${B} ; ${AR}	\
		${D}README	${D}COPYRIGHT	${D}Makefile	${D}*.c \
	> ${D}misc.shar )

adldebug	:	adldebug.o	${UTIL}
	$(CC) $(CFLAGS) -o $@ adldebug.o ${UTIL}

adldebug.o	:	adldebug.c \
			${INC}/adltypes.h	${INC}/adlprog.h \
			${INC}/adldef.h		${INC}/vstring.h \
			${INC}/virtmem.h

adltouch	:	adltouch.o
	$(CC) $(CFLAGS) -o $@ adltouch.o

adltouch.o	:	adltouch.c
