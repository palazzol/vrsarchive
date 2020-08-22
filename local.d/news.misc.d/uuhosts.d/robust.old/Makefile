#
#	uuhosts %W%	%G%
#
#	nmail.paths is found in $(LIB)
#	Most news parameter files and mapsh are in $(NEWS)
#	Maps from mod.map.all are in $(MAPS)
#
SHELL	= /bin/sh
LIB	= /usr/local/lib
NEWS	= $(LIB)/news
MAPS	= $(NEWS)/maps
SOURCES	= uuhosts.sh mapsh.c news.to.uu.sh
ALL	= uuhosts mapsh uucp.from.news

all: $(ALL)

uuhosts: uuhosts.sh
	sed -e \
's%^LIB=.*$$%LIB=$(LIB)%;s%^NEWS=.*$$%NEWS=$(NEWS)%;s%^MAPS=.*$$%MAPS=$(MAPS)%'\
 uuhosts.sh > uuhosts

mapsh: mapsh.c
	$(CC) -o mapsh -DMAPS=\"$(MAPS)\" mapsh.c

uucp.from.news: news.to.uu.sh
	sed -e \
's%^LIB=.*$$%LIB=$(LIB)%;s%^NEWS=.*$$%NEWS=$(NEWS)%;s%^MAPS=.*$$%MAPS=$(MAPS)%'\
 news.to.uu.sh > uucp.from.news

install: mapsh
	cp uuhosts /usr/local/uuhosts
	rm $(NEWS)/mapsh
	cp mapsh $(NEWS)/mapsh
	strip $(NEWS)/mapsh
	chown root $(NEWS)/mapsh
	chgrp news $(NEWS)/mapsh
	chmod 4550 $(NEWS)/mapsh
	ls -lg $(NEWS)/mapsh

clean:
	rm -f $(ALL)
	rm -f *.o
	rm -f *.shar

shar:
	shar README Makefile $(SOURCES) > uuhosts.shar
