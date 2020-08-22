# makefile for C news relaynews

# define NOSTOREVAL if your dbm store() returns no value (as in orig. v7)
# -I. for fcntl.h if present
DEFINES= -I../include -I. -DSTATIC=
# -Dvoid=int			# if your compiler doesn't understand void's
# -DMAXLONG=017777777777L	# if your compiler lacks "unsigned long" type
# -Dindex=strchr -Drindex=strrchr	# if not on (System III or V or PWB)

CFLAGS=-O -f68881 $(DEFINES) # -pg # -Z: John Bruner's Z0MAGIC (unmapped first page)
LIBS= -ldbm # -lstdio
LLIBS=-llocal
PROPTS=-l132
P=impr -p

POSSDEPFILES=fcntl.h gethostname.c getwd.c dbm.c mkdir.c ftime.c clsexec.c memcpy.c
# adjust next three lines for your OS; sources are links to vers/*/*.c
DEPHDR=
DEPSRC= clsexec.c memcpy.c zeropad.c
DEPOBJ= clsexec.o memcpy.o zeropad.o

LIBSRCS= ../libcnews/*.c
LIBOBJS= ../libcnews/libcnews.a ../libc/libc.a
SRC=relaynews.c active.c caches.c checkdir.c control.c fileart.c \
	headers.c history.c hostname.c io.c msgs.c procart.c \
	string.c sys.c transmit.c $(DEPSRC) $(LIBSRCS)
OBJ=relaynews.o active.o caches.o checkdir.o control.o fileart.o \
	headers.o history.o hostname.o io.o msgs.o procart.o \
	string.o sys.o transmit.o $(DEPOBJ) $(LIBOBJS)
FILES=$(NONCFILES) $(CFILES)
NONCFILES= TODO* COPYRIGHT README ads/README ads/[0-9]* \
	inews anne.jones tear rnews makefile
CFILES= ../include/*.h cpu.h headers.h system.h $(DEPHDR) $(SRC)
LINT=lint
LINTFLAGS=-haz $(DEFINES)

relaynews: $(OBJ) ../libcnews/*.c
	(cd ../libcnews; make)
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $@
lint: $(SRC)
	$(LINT) $(LINTFLAGS) $(SRC) $(LIBS) $(LLIBS) | egrep -v ':$$'
lintport: $(SRC)
	$(LINT) $(LINTFLAGS) -p $(SRC) $(LIBS) $(LLIBS)

TODO.grep: TODO
	-egrep TODO TODO ../include/*.h *.h *.c sh/* >$@
	-egrep TODO ../libc/memcpy.fast/src/*.c ../libcnews/*.c >>$@

v7 v8 v9 usg bsd42: clean
	for file in vers/$@/*.c; do ln $$file; done
	@echo 'Now edit makefile DEPSRC and DEPOBJ definitions to match:'
	@echo vers/$@/*.c | sed 's;vers/$@/;;g'

print: printc printnonc
	touch $@
printc: $(CFILES)
	/usr/bin/pp -T300 -fR $? | dimp -t | impr
	touch $@
printnonc: $(NONCFILES)
	pr $(PROPTS) $? | $P
	touch $@
distr: $(FILES)
	(echo relaynews update of `date`; echo ""; bundle $?) | /bin/mail cnews-updates
	touch $@
clean:
	rm -f core a.out relaynews *.o $(POSSDEPFILES)

# header dependencies follow
active.o:	active.h ../include/news.h ../include/newspaths.h
caches.o:	../include/news.h
checkdir.o:	../include/news.h
control.o:	../include/news.h ../include/newspaths.h headers.h history.h
fileart.o:	../include/news.h ../include/newspaths.h active.h headers.h system.h
headers.o:	../include/news.h headers.h
history.o:	../include/news.h ../include/newspaths.h headers.h
hostname.o:	../include/news.h ../include/newspaths.h
io.o:		../include/news.h
msgs.o:		../include/news.h
procart.o:	../include/news.h active.h headers.h system.h
relaynews.o:	../include/news.h ../include/newspaths.h active.h cpu.h headers.h system.h
string.o:	../include/news.h
sys.o:		../include/news.h ../include/newspaths.h system.h
transmit.o:	../include/news.h ../include/newspaths.h headers.h system.h
