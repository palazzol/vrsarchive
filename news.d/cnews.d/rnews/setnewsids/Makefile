# makefile for C news setnewsids

# define NOSTOREVAL if your dbm store() returns no value (as in orig. v7)
# -I. for fcntl.h if present
DEFINES= -I../../include -I../. -DSTATIC=
# -Dvoid=int			# if your compiler doesn't understand void's
# -DMAXLONG=017777777777L	# if your compiler lacks "unsigned long" type
# -Dindex=strchr -Drindex=strrchr	# if not on (System III or V or PWB)

CFLAGS=-O $(DEFINES) # -pg # -Z: John Bruner's Z0MAGIC (unmapped first page)
LIBS= ../memcpy.o ../../libcnews/*.a -ldbm ../../libc/*.a # -lstdio
LLIBS=-llocal
PROPTS=-l132
P=impr -p

POSSDEPFILES=fcntl.h gethostname.c getwd.c dbm.c mkdir.c ftime.c clsexec.c memcpy.c
# adjust next three lines for your OS; sources are links to vers/*/*.c
DEPHDR=
DEPSRC= clsexec.c memcpy.c zeropad.c
DEPOBJ= clsexec.o memcpy.o zeropad.o

LIBSRCS= ../../libcnews/*.c
LIBOBJS= ../../libcnews/../libcnews.a ../../libc/../libc.a
SRC=setnewsids.c
OBJ=setnewsids.o
FILES=$(SRC)
LINT=lint
LINTFLAGS=-haz $(DEFINES)

all: setnewsids

TODO.grep: TODO
	-egrep TODO TODO ../../include/*.h *.h *.c sh/* >$@
	-egrep TODO ../../libc/memcpy.fast/src/*.c ../../libcnews/*.c >>$@

v7 v8 v9 usg bsd42: clean
	for file in vers/$@/*.c; do ln $$file; done
	@echo 'Now edit makefile DEPSRC and DEPOBJ definitions to match:'
	@echo vers/$@/*.c | sed 's;vers/$@/;;g'

setnewsids: $(OBJ) ../../libcnews/*.c
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $@
lint: $(SRC)
	$(LINT) $(LINTFLAGS) $(SRC) $(LIBS) $(LLIBS) | egrep -v ':$$'
lintport: $(SRC)
	$(LINT) $(LINTFLAGS) -p $(SRC) $(LIBS) $(LLIBS)

print: $(FILES)
	pr $(PROPTS) $? | $P
	touch print
distr: $(FILES)
	(echo setnewsids update of `date`; echo ""; bundle $?) | /bin/mail cnews-updates
	touch distr
clean:
	rm -f core a.out setnewsids *.o $(POSSDEPFILES)

# header dependencies follow
setnewsids.o: ../../include/news.h ../../include/newspaths.h
active.o: ../../include/news.h
control.o: ../../include/news.h headers.h
fileart.o: ../../include/news.h headers.h
headers.o: ../../include/news.h headers.h
history.o: ../../include/news.h headers.h
hostname.o: ../../include/news.h
sys.o: ../../include/news.h system.h
transmit.o: ../../include/news.h headers.h system.h
