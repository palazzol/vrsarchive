#  - un'cpio' this in /usr/src or /usr/local/src or something - doesn't matter.
#  - do a 'make install' in the tetrix directory
#  - tetrix gets installed in /usr/lbin
#  - this will create a high score file in /usr/tmp, so doing it again
#    later on will erase high scores for the machine.
ALL	= tetrix$(EXE)
MODEL	= -i
OPTIONS	= -DSCORE_FILE=\"$(GAMLIB)/tetrix\"
include $(HOME)/CONFIG
OBJS	= MoveR.o MoveL.o NewP.o AdvanceP.o Rotate.o tet.o
INCS	= tet.h

tetrix$(EXE): $(OBJS) $(INCS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

install: tetrix
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	touch $(GAMLIB)/tetrix
	chmod 666 $(GAMLIB)/tetrix

clean:
	rm -rf $(ALL) core $(OBJS)
