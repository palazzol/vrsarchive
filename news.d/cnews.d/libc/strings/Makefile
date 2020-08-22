# String library.

# Configuration settings:  how should "size_t", "void *", "const" be written?
# "size_t" is what's needed to hold the result of sizeof; beware of problems
# with compatibility here, because X3J11 uses this for e.g. the third
# argument of strncpy() as well.  You may need to make it "int" even if
# this is a lie.  "void *" is the generic pointer type, "char *" in most
# existing implementations.  "const" is the keyword marking read-only
# variables and parameters, unimplemented in most existing implementations.
# These things need to be defined this way because they must be fitted into
# both the .h files and the .c files; see the make instructions for string.h
# farther down.
SIZET = int
VOIDSTAR = char *
LVOIDSTAR = char*	# Lint shell file has problems with * alone.  Barf.
CONST = 

CONF = -DSIZET=$(SIZET) -DVOIDSTAR='$(VOIDSTAR)' -DCONST='$(CONST)'
LCONF = -DSIZET=$(SIZET) -DVOIDSTAR='$(LVOIDSTAR)' -DCONST='$(CONST)'

# Things you might want to put in CFLAGS or LINTFLAGS.
# -DCHARBITS=0377		Required if compiler lacks "unsigned char".
# -Dvoid=int			Required if compiler lacks "void".
# -DUNIXERR			Unix-like errno stuff, can test strerror().
# -DBERKERR			Like UNIXERR but for Berklix (4BSD).
# -I.				string.h from here, not /usr/include.

CFLAGS = -O $(CONF) -DUNIXERR -I.
LINTFLAGS = -hpan $(LCONF) -DUNIXERR -Dvoid=int -DCHARBITS=0377 -I.
LDFLAGS = -i

# Name lists.
STRING = index.o rindex.o strcat.o strchr.o strcmp.o strcpy.o strcspn.o \
	strlen.o strncat.o strncmp.o strncpy.o strpbrk.o strrchr.o strspn.o \
	strtok.o strstr.o memcpy.o memccpy.o memcmp.o memchr.o memset.o \
	bcopy.o bcmp.o bzero.o strerror.o
CSTRING = index.c rindex.c strcat.c strchr.c strcmp.c strcpy.c strcspn.c \
	strlen.c strncat.c strncmp.c strncpy.c strpbrk.c strrchr.c strspn.c \
	strtok.c strstr.c memcpy.c memccpy.c memcmp.c memchr.c memset.c \
	bcopy.c bcmp.c bzero.c strerror.c
DTR = README Makefile $(CSTRING) tester.c string.h.proto

# Locations, for installation (somewhat system-dependent).
DEST=..

tester.o:	string.h

mv:	$(STRING)
	mv $(STRING) $(DEST)

r:	tester
	@echo 'No news is good news.  Note: strerror() test is VERY system-dependent.'
	tester

tester:	tester.o $(STRING)
	cc $(LDFLAGS) tester.o $(STRING) -o tester

string.h:	string.h.proto
	sed 's/SIZET/$(SIZET)/g;s/VOIDSTAR /$(VOIDSTAR)/g' string.h.proto >string.h

memory.h:	string.h
	egrep mem string.h >memory.h

lint:	string.h
	lint $(LINTFLAGS) tester.c $(CSTRING)

clean:
	rm -f tester a.out *.o string.h memory.h dtr

dtr:	$(DTR)
	makedtr $(DTR) >dtr
