# makefile for hbanner & mk_font

HERSHDIR = /stuff/hershey
FONTDIR = /local/lib/hbfonts

CFLAGS= -O

all: hbanner mk_font

hbanner: hbanner.c
	cc -o hbanner $(CFLAGS) -DFONTDIR=\"$(FONTDIR)\" hbanner.c

mk_font: mk_font.sh
	sed -e "s|%HERSHDIR%|$(HERSHDIR)|" -e "s|%FONTDIR%|$(FONTDIR)|" \
		< mk_font.sh > mk_font
	chmod 775 mk_font

shar: all
	shar README Makefile mk_font.sh hbanner.c > hbanner.shar
