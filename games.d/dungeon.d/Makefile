# Makefile for creating dungeon
ALL	= dungeon$(EXE)
FC	= g77 -ff77 -fugly-assumed -fugly-logint
OPTIONS	= -DSYSV
# Directory path for data files
DDIR = $(GAMLIB)/dunlib
# userid for wizard - allows invoking game debugging tool
FFLAGS = -O -DSYSV -DDDIR=\'$(DDIR) -DWIZARDID=500
include $(HOME)/CONFIG

.SUFFIXES: .uu

.uu:
	uudecode $<

OBJS =  actors.o ballop.o clockr.o demons.o\
	dgame.o dinit.o dmain.o dso1.o dso2.o\
	dso3.o dso4.o dso5.o dso6.o dso7.o\
	dsub.o dverb1.o dverb2.o gdt.o lightp.o\
	nobjs.o np.o np1.o np2.o np3.o nrooms.o objcts.o\
	rooms.o rtim.o sobjs.o sverbs.o verbs.o villns.o

clean:
	$(RM) $(OBJS) $(ALL)

dungeon$(EXE): $(OBJS)
	$(FC) -s -o $@ $(OBJS)

$(DDIR):
	mkdir -p $@

install: $(ALL) dtext.dat $(DDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cp dindx.dat dtext.dat $(DDIR)
	cp dungeon.6 $(MAN6)

#dtext.dat: 
#	./maketxt.sh
