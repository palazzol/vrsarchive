#
#	Worms %W% %G%
#
ALL	= worms
OPTIONS	= -O
MODEL	= -i -F 2000
LFLAGS	= -h
include /etc/CONFIG


lint: ${CFILES} 
	lint ${LFLAGS} ${CFILES} ${LIBS} 

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f ${OBJS} make.out core worms
