ALL		= omega
MODEL	= -M2lt20
OPTIONS	= -DEXCESSIVE_REDRAW -DOMEGALIB=\"$(OMEGALIB)/\"
include $(HOME)/CONFIG
OMEGALIB= $(GAMLIB)/omega
CONFIGO	= oabyss.o ocity.o ocom3.o ocountry.o oenv.o ofile.o ohouse.o omon.o \
		  ovillage.o
SUPPORT	= oabyss.dat oabyss.txt oarena.dat occmds.txt ocircle.dat ocmds.txt \
		  ocountry.dat ocourt.dat odlair.dat ohelp*.txt ohome*.dat \
		  olicense.txt omaze*.dat omega.hi omisle.dat omotd.txt oscroll*.txt \
		  ospeak.dat otemple.dat ovillage*.dat ointro.txt ocity.dat
WENABLE	= omega.hi
OBJS	= o.o oabyss.o oaux1.o oaux2.o oaux3.o ochar.o ocity.o ocom1.o \
		  ocom2.o ocom3.o ocountry.o oeffect1.o oeffect2.o oeffect3.o \
		  oetc.o oenv.o ofile.o ogen1.o ogen2.o oguild1.o oguild2.o ohouse.o \
		  oinv.o oitem.o oitemf1.o oitemf2.o oitemf3.o olev.o ommelee.o \
		  ommove.o omon.o omove.o omovef.o omspec.o omstrike.o omtalk.o \
		  opriest.o osave.o oscr.o osite1.o osite2.o ospell.o otime.o otrap.o \
		  outil.o ovillage.o

omega: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LIBS)

clean:
	rm -rf *.o omega

$(OMEGALIB): $(GAMLIB)
	mkdir $@
	chmod 755 $@

install: omega $(OMEGALIB)
	cp omega $(GAMDIR)
	cd $(GAMDIR); chmod 711 omega
	cp $(SUPPORT) $(OMEGALIB)
	cd $(OMEGALIB); chmod a=r,u+w $(SUPPORT)
	cd $(OMEGALIB); chmod a+rw $(WENABLE)
	cd $(OMEGALIB); touch omega.log
	cd $(OMEGALIB); chmod a+rw omega.log

$(OBJS): odefs.h oglob.h
