#    Makefile for ARC

CFLAGS = -O -M2l

OBJS = arc.o arcadd.o arccode.o arccvt.o arcdel.o arcdos.o \
       arcext.o arcio.o arclst.o arclzw.o arcmatch.o arcmisc.o \
       arcpack.o arcsq.o arcsvc.o arctst.o arcunp.o arcusq.o

SRCS = arc.c arcadd.c arccode.c arccvt.c arcdel.c arcdos.c \
       arcext.c arcio.c arclst.c arclzw.c arcmatch.c arcmisc.c \
       arcpack.c arcsq.c arcsvc.c arctst.c arcunp.c arcusq.c

arc:            ${OBJS}
	cc  ${CFLAGS} -o arc ${OBJS}
${OBJS}:        arc.h
arc.h:          arcs.h
	touch arc.h

lint:
	lint $(SRCS) | tee lint.out

clean:
	rm -f arc $(OBJS)
