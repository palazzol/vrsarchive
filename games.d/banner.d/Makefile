# makefile for hbanner & mk_font
ALL	= hbanner$(EXE) mk_font
OPTIONS	= -O
HERSHDIR = ./hershey-font
include $(HOME)/CONFIG
FONTDIR = $(LIBDIR)/hbfonts

clean:
	rm -f $(ALL)

hbanner$(EXE): hbanner.c
	$(CC) -o hbanner $(CFLAGS) -DFONTDIR=\"$(FONTDIR)\" hbanner.c

mk_font: mk_font.sh
	sed -e "s|%HERSHDIR%|$(HERSHDIR)|" -e "s|%FONTDIR%|$(FONTDIR)|" \
		< mk_font.sh > mk_font
	chmod 755 mk_font

install: $(GAMDIR)
	cp hbanner$(EXE) $(GAMDIR)
	cd $(GAMDIR); chmod 755 hbanner$(EXE) 
	mkdir -p $(FONTDIR)
	for i in cyrilc gothgbt gothgrt gothitt greekc greekcs greekp greeks \
		italicc italiccs italict romanc romancs romand romanp romans \
		romant scriptc scripts; do \
	  ./mk_font $$i; \
	done

shar: all
	shar README Makefile mk_font.sh hbanner.c > hbanner.shar
