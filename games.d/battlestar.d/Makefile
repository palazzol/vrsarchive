#
#	Battelstar %W% %G%
#
ALL	= battlestar
MODEL	= -M2l
OPTIONS	= -O -DGAMLIB=\"$(GAMLIB)
include /etc/CONFIG
LOGFILE	= $(GAMLIB)/battlestar.log
DAYF	= $(GAMLIB)/dayfile
NIGHTF	= $(GAMLIB)/nightfile
MAN	= $(MAN6)/battlestar.6
OBJS	= battlestar.o com1.o com2.o com3.o com4.o com5.o com6.o \
	  com7.o init.o cypher.o getcom.o parse.o room.o save.o fly.o \
	  misc.o globals.o wlist.o dayobjs.o nightobjs.o initrms.o
CFILES	= battlestar.c com1.c com2.c com3.c com4.c com5.c com6.c \
	  com7.c init.c cypher.c getcom.c parse.c room.c save.c fly.c \
	  misc.c globals.c initrms.c
LFLAGS	= -h
JUNKF	= $(ALL) tags mkfile mkwlist mkobjs \
	  wlist.c dayobjs.c nightobjs.c mkwlist.o mkobjs.o
DATAF	= dayfile nightfile battlewords dayobjs nightobjs

battlestar: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

$(DATAF): ;

mkfile: mkfile.c
	$(CC) $(CFLAGS) -o mkfile mkfile.c

mkwlist: mkwlist.c
	$(CC) $(CFLAGS) -o mkwlist mkwlist.c

wlist.c: battlewords mkwlist
	mkwlist battlewords

mkobjs: mkobjs.c
	$(CC) $(CFLAGS) -o mkobjs mkobjs.c

dayobjs.c: dayobjs mkobjs
	mkobjs dayobjs

nightobjs.c: nightobjs mkobjs
	mkobjs nightobjs

lint: $(CFILES) 
	lint $(LFLAGS) $(CFILES) $(LIBS) 

install: $(GAMDIR)/battlestar $(MAN) $(LOGFILE) $(DAYF) $(NIGHTF)

$(GAMDIR)/battlestar: $(GAMDIR) battlestar
	cp battlestar $(GAMDIR)
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
	rm -f $(OBJS) $(JUNKF)
