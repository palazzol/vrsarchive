#
#	Battelstar %W% %G%
#
ALL	= battlestar$(EXE)
MODEL	= -M2l
OPTIONS	= -O -DGAMLIB=$(GAMLIB)
include $(HOME)/CONFIG
LOGFILE	= $(GAMLIB)/battlestar.log
DAYF	= $(GAMLIB)/dayfile
NIGHTF	= $(GAMLIB)/nightfile
MAN	= $(MAN6)/battlestar.6
OBJS	= battlestar.o _com1.o _com2.o _com3.o _com4.o _com5.o _com6.o \
	  _com7.o init.o cypher.o getcom.o parse.o room.o save.o fly.o \
	  misc.o globals.o wlist.o dayobjs.o nightobjs.o initrms.o
CFILES	= battlestar.c _com1.c _com2.c _com3.c _com4.c _com5.c _com6.c \
	  _com7.c init.c cypher.c getcom.c parse.c room.c save.c fly.c \
	  misc.c globals.c initrms.c
LFLAGS	= -h
JUNKF	= $(ALL) tags mkfile mkwlist mkobjs \
	  wlist.c dayobjs.c nightobjs.c mkwlist.o mkobjs.o
DATAF	= dayfile nightfile battlewords dayobjs nightobjs

battlestar$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

$(DATAF): ;

mkfile$(EXE): mkfile.c
	$(CC) $(CFLAGS) -o mkfile mkfile.c

mkwlist$(EXE): mkwlist.c
	$(CC) $(CFLAGS) -o mkwlist mkwlist.c

wlist.c: battlewords mkwlist
	./mkwlist battlewords

mkobjs$(EXE): mkobjs.c
	$(CC) $(CFLAGS) -o mkobjs mkobjs.c

dayobjs.c: dayobjs mkobjs
	./mkobjs dayobjs

nightobjs.c: nightobjs mkobjs
	./mkobjs nightobjs

lint: $(CFILES) 
	lint $(LFLAGS) $(CFILES) $(LIBS) 

install: $(GAMDIR)/battlestar$(EXE) $(MAN) $(LOGFILE) $(DAYF) $(NIGHTF)

$(GAMDIR)/battlestar$(EXE): $(GAMDIR) battlestar$(EXE)
	cp battlestar$(EXE) $(GAMDIR)
	chown $(SAFEUID) $@
	chgrp $(SAFEGRP) $@
	chmod 4711 $@

$(DAYF): $(GAMLIB) dayfile
	cp dayfile $@
	chown $(SAFEUID) $@
	chgrp $(SAFEGRP) $@
	chmod 600 $@

$(NIGHTF): $(GAMLIB) nightfile
	cp nightfile $@
	chown $(SAFEUID) $@
	chgrp $(SAFEGRP) $@
	chmod 600 $@

$(LOGFILE): $(GAMLIB)
	touch $(LOGFILE)
	chown $(SAFEUID) $@
	chgrp $(SAFEGRP) $@
	chmod 644 $(LOGFILE)

$(MAN): $(MAN6) battlestar.6
	cp battlestar.6 $@
	chmod 644 $@

clean:
	rm -f $(OBJS) $(JUNKF) $(ALL)
	rm -f mkobjs$(EXE) mkwlist$(EXE) mkfile$(EXE)
