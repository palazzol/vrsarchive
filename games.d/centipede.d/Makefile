#
# makefile for centipede
# edit sys_dep.c to adjust pathnames for your system
#
ALL	= cent
OPTIONS	= -O -DGAMLIB=$(GAMLIB)/cent
MODEL	= -i -F 2000
include /etc/CONFIG
OBJECTS	= cent.o stuff.o input.o interrupts.o score.o move.o rand.o \
		save.o shoot.o la.o extern.o options.o sys_dep.o

cent:   $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

$(GAMLIB)/cent:
	mkdir $@
	chown $(SAFEUID) $@
	chmod 755 $@

install: $(ALL) $(GAMLIB)/cent
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 4711 $(ALL)
	cp cent.doc $(GAMLIB)/cent
	cd $(GAMLIB)/cent; touch scorefile 
	cd $(GAMLIB)/cent; chown $(SAFEUID) scorefile cent.doc
	cd $(GAMLIB)/cent; chmod 644 scorefile cent.doc

clean:
	rm -f $(ALL) $(OBJECTS)

cent.o stuff.o input.o move.o shoot.o save.o extern.o options.o: cent.h
input.o options.o: sys_dep.h
sys_dep.o: Makefile
