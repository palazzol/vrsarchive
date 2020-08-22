#
#    Makefile for ARC
#
ALL	= arc$(EXE)
OPTIONS	= -O
MODEL	= -M2l
include $(HOME)/CONFIG
OBJS = arc.o arcadd.o arccode.o arccvt.o arcdel.o arcdos.o \
       arcext.o arcio.o arclst.o arclzw.o arcmatch.o arcmisc.o \
       arcpack.o arcsq.o arcsvc.o arctst.o arcunp.o arcusq.o
SRCS = $(OBJS:.o=.c)

arc$(EXE):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS):arc.h
arc.h:	arcs.h
	touch arc.h

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)/
	cd $(BINDIR)/; chmod 711 $(ALL)

lint:
	lint $(SRCS) | tee lint.out

clean:
	rm -f $(ALL) $(OBJS)
