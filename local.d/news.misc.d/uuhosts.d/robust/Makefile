# @(#) Makefile 1.26 85/09/17
SHELL=/bin/sh
HOME=/user/mkt/vrs
# Settable parameters.

# Default pager for use if PAGER environment variable is not set.
DEFPAGER=more	# BSD systems
#DEFPAGER=pg	# AT&T systems
#DEFPAGER=cat	# systems with paging in the kernel

# BIN should be either /usr/local or /usr/local/bin
BIN=$(HOME)/bin
LIB=$(HOME)/lib
PATHS=$(LIB)/nmail.paths

# If you do not have dbm(3), i.e., -ldbm, set UUWHERE to nothing.
UUWHERE=uuwhere
# UUWHERE=
WHERE=$(LIB)/nmail.where
# TMP should be in the same directory as WHERE.
TMP=$(LIB)/nmail.tmp
# -ldbm DATABASE used by uuwhere.
DATABASE=$(LIB)/uuindex

# Most of the news parameter files and mapsh are in this directory.
NEWS=$(LIB)/news
# The maps from mod.map are under this directory.
# (Another good place to put it is MAPS=/usr/spool/news/maps.)
MAPS=$(NEWS)/maps

# The filename of the manual entry.
UUHOSTSMAN=uuhosts.1
UUWHEREMAN=uuwhere.1
MAN=$(UUHOSTSMAN) $(UUWHEREMAN)
MANSOURCES=uuhosts.man uuwhere.man
# The manual directory for local entries.
# MANDIR=/usr/man/manl
MANDIR = /tmp

# The rest of the Makefile shouldn't need to be changed.

UUCPMAPGROUP=mod.map
# This can't be the same as UUCPMAPGROUP
OLDUUCPMAPGROUP=mod.map.uucp
# USENET spool directory containing articles from newsgroup $(UUCPMAPGROUP).
# Must match $(UUCPMAPGROUP)!
# UUCPSPOOL=/usr/spool/news/mod/map
UUCPSPOOL=$(HOME)/news/mod/map
# The place uuhosts stores the $(UUCPMAPGROUP) information for later retrieval.
UUCPMAP=$(MAPS)/$(UUCPMAPGROUP)

SOURCES=uuhosts.sh mapsh.c uuwhere.c $(MANSOURCES)
ALL= uuhosts mapsh $(UUWHERE) $(MAN)

all: $(ALL)

uuhosts: uuhosts.sh
	cat uuhosts.sh \
	| sed -e 's%^DEFPAGER=.*$$%DEFPAGER=$(DEFPAGER)%' \
	| sed -e 's%^UUCPMAPGROUP=.*$$%UUCPMAPGROUP=$(UUCPMAPGROUP)%' \
	| sed -e 's%^UUCPSPOOL=.*$$%UUCPSPOOL=$(UUCPSPOOL)%' \
	| sed -e 's%^BIN=.*$$%BIN=$(BIN)%;s%^LIB=.*$$%LIB=$(LIB)%' \
	| sed -e 's%^PATHS=.*$$%PATHS=$(PATHS)%;s%^WHERE=.*$$%WHERE=$(WHERE)%'\
	| sed -e 's%^NEWS=.*$$%NEWS=$(NEWS)%;s%^MAPS=.*$$%MAPS=$(MAPS)%'\
	> uuhosts
	chmod +x uuhosts

mapsh: mapsh.c
	$(CC) -o mapsh -DMAPS=\"$(MAPS)\" mapsh.c

uuwhere: uuwhere.c
	$(CC) -o uuwhere -DUUCPINDEX=\"$(UUCPMAP)/Index\" \
			-DPATHS=\"$(PATHS)\" -DWHERE=\"$(WHERE)\" \
			-DTMP=\"$(TMP)\" -DDATABASE=\"$(DATABASE)\" \
			uuwhere.c -ldbm

where:	$(LIB)/uuwhere

$(UUHOSTSMAN):	uuhosts.man
	sed -e 's%UUCPMAPGROUP%$(UUCPMAPGROUP)%;s%UUCPMAP%$(UUCPMAP)%' uuhosts.man \
	| sed -e 's%DEFPAGER%$(DEFPAGER)%;s%BIN%$(BIN)%;s%LIB%$(LIB)%' \
	| sed -e 's%PATHS%$(PATHS)%;s%WHERE%$(WHERE)%;s%TMP%$(TMP)%'\
	| sed -e 's%DATABASE%$(DATABASE)%;s%NEWS%$(NEWS)%;s%MAPS%$(MAPS)%'\
		> $(UUHOSTSMAN)

$(UUWHEREMAN):	uuwhere.man
	sed -e 's%UUCPMAP%$(UUCPMAP)%' uuwhere.man \
	| sed -e 's%BIN%$(BIN)%;s%LIB%$(LIB)%' \
	| sed -e 's%PATHS%$(PATHS)%;s%WHERE%$(WHERE)%;s%TMP%$(TMP)%'\
	| sed -e 's%DATABASE%$(DATABASE)%;s%NEWS%$(NEWS)%;s%MAPS%$(MAPS)%'\
		> $(UUWHEREMAN)

inst_:

inst_uuwhere: $(UUWHERE)
	-rm -f $(LIB)/uuwhere
	cp uuwhere $(LIB)/uuwhere
	chmod 755 $(LIB)/uuwhere
	cp /dev/null $(WHERE)
	-chmod 666 $(WHERE)
	-rm -f $(MANDIR)/$(UUWHEREMAN)
	cp $(UUWHEREMAN) $(MANDIR)
	chmod 444 $(MANDIR)/$(UUWHEREMAN)

install: $(ALL) inst_$(UUWHERE) inst_most recover

inst_most:
	-rm -f $(BIN)/uuhosts
	cp uuhosts $(BIN)/uuhosts
	chmod 755 $(BIN)/uuhosts
	-rm -f $(MANDIR)/$(UUHOSTSMAN)
	cp $(UUHOSTSMAN) $(MANDIR)
	chmod 444 $(MANDIR)/$(UUHOSTSMAN)
	@echo "Rest of installation has to be done as root."
	cp mapsh $(NEWS)/mapsh
	-strip $(NEWS)/mapsh
	-chown root $(NEWS)/mapsh
	-chgrp news $(NEWS)/mapsh
	-chmod 4750 $(NEWS)/mapsh
	ls -lg $(NEWS)/mapsh
	-mkdir $(MAPS)
	-mkdir $(MAPS)/bin $(MAPS)/tmp $(UUCPMAP)
	-chgrp news $(MAPS) $(MAPS)/*
	chmod 755 $(MAPS)
	chmod 775 $(UUCPMAP)
	chmod 777 $(MAPS)/tmp
	chmod 755 $(MAPS)/bin
	cp /bin/cat $(MAPS)/bin
	cp /bin/echo $(MAPS)/bin
	cp /bin/sed $(MAPS)/bin
	cp /bin/sh $(MAPS)/bin
	-strip $(MAPS)/bin/*
	-chgrp news $(MAPS)/bin/*
	chmod 755 $(MAPS)/bin/*
	@echo ''
	@echo 'This is what the things just installed *should* look like:'
	@echo '$(MAPS): total 16'
	@echo 'dr-xr-xr-x  6 root     news          512 Nov 11 16:42 .'
	@echo 'drwxrwxr-x 19 news     news         1024 Nov 11 16:45 ..'
	@echo 'dr-xr-xr-x  2 root     news          512 Nov 11 16:39 bin'
	@echo 'drwxrwxr-x  2 news     news        10240 Nov 11 16:39 mod.map'
	@echo 'drwxrwxrwx  2 news     news           24 Nov 11 16:41 tmp'
	@echo ''
	@echo '$(MAPS)/bin: total 59'
	@echo '-r-xr-xr-x  1 root     news        10240 Nov 11 15:29 cat'
	@echo '-r-xr-xr-x  1 root     news         4096 Nov 11 16:33 echo'
	@echo '-r-xr-xr-x  1 root     news        18432 Nov 11 15:29 sed'
	@echo '-r-xr-xr-x  1 root     news        27648 Nov 11 15:29 sh'
	@echo ''
	@echo 'And here is what they actually *do* look like:'
	ls -lga $(MAPS)
	ls -lga $(MAPS)/bin

recover:
	@echo "Attempt to recover map data from a previous newsgroup ("$(OLDUUCPMAPGROUP)")."
	@echo "If it fails, you probably don't have any old map data."
	mkdir /tmp/$(OLDUUCPMAPGROUP)
	-(cd /tmp/$(OLDUUCPMAPGROUP); cd $(MAPS)/$(OLDUUCPMAPGROUP); \
		mv * ../$(UUCPMAPGROUP); cd ..;  rmdir $(OLDUUCPMAPGROUP))
	rmdir /tmp/$(OLDUUCPMAPGROUP)

clean:
	rm -f $(ALL)
	rm -f *.pag *.dir
	rm -f *.o
	rm -f *.shar

shar:
	shar README* $(SOURCES) Makefile > uuhosts.shar
