# makefile for newshist
RN=../rnews
CFLAGS=-f68881 -I$(RN)	# for history.h
RNEWSOBJS=$(RN)/history.o $(RN)/io.o $(RN)/msgs.o \
	  $(RN)/clsexec.o $(RN)/memcpy.o $(RN)/string.o
LIBOBJS= ../libcnews/*.a ../libc/*.a
LIBS=-ldbm

newshist: newshist.o $(RNEWSOBJS)
	$(CC) $(CFLAGS) -o $@ newshist.o $(RNEWSOBJS) $(LIBOBJS) $(LIBS)

clean:
	rm -f *.o newshist
