#
# Rewritten Makefile (Use CONFIG) by Vincent Slyngstad, 1/22/88
#
ALL	= omega
OMEGALIB= $(GAMLIB)/omega
OPTIONS	= -O -DOMEGALIB=\"$(OMEGALIB)/\" -DWIZARD=\"root\"
include /etc/CONFIG
OBJ	= o.o oaux1.o oaux2.o ochar.o ocity.o ocom.o odepths.o oeffect1.o \
	  oeffect2.o oetc.o ofile.o ogen.o oguild.o oinititem1.o oinititem2.o \
	  oinimon0to3.o oinimon4to7.o oinimon8to10.o oinv.o oitem.o oitemf.o \
	  olev.o omon.o omonf.o omove.o oscr.o osite.o ospell.o otime.o outil.o
AUXFILES= ocity.dat ocommands.txt odepths.dat ohelp.txt omega.hiscore \
	  omega.intro omega.log omega.lognum omega.motd \
	  oscroll1.txt oscroll2.txt
RW	= $(OMEGALIB)/omega.saves $(OMEGALIB)/omega.hiscore \
	  $(OMEGALIB)/omega.log $(OMEGALIB)/omega.lognum

omega:	$(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ) $(LIBS)

$(OMEGALIB):
	mkdir $@
	chmod 755 $@

install: $(ALL) $(AUXFILES) $(GAMDIR) $(GAMLIB) $(OMEGALIB)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp $(AUXFILES) $(OMEGALIB)
	touch $(OMEGALIB)/omega.saves
	chmod 666 $(RW)

deinstall: $(GAMDIR) $(GAMLIB) $(OMEGALIB)
	cd $(GAMDIR); rm -f $(ALL)
	cd $(OMEGALIB); rm -f $(AUXFILES) omega.saves

clean:
	rm -f $(OBJ) $(ALL)
