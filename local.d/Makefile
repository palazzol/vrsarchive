#
#	$Header: /home/Vince/cvs/local.d/Makefile,v 1.5 1987-07-26 13:24:39 vrs Exp $
#
MODEL	= -M2em -F 2000
OPTIONS	= -DMAP=\"$(LIBDIR)/uucp.map\"
TODO	= LaTeX.d dialers.d lbgm.d map.d news.d news.misc.d
SUBDIR	= arc ash arcmail calend calls car clear compress copymode crack ctc \
	  ctty curse cxref displays dithds elbat esad espel getopt hoc6 \
	  include index jove kermit less lintlib mdemo mkman ofiles patch \
	  pathalias read20 recipes roff se strings teco tmac talk top tpr \
	  umodem vc vttest whatcomm which xmodem
M0700	= bootime logout mkbad
M0711	= bincmp clock from getfrom script
M0755	= backup datefudge dirsort dtgt dtree getto islide lpr makewhatis \
	  phc ports slide vslide
M4711	= cbuf cmap smap
FORUUCP	= dial.onet
ALL	= $(SUBDIR) $(SCRIPTS) $(M0700) $(M0711) $(M0755) $(M4711) $(FORUUCP)
include ../CONFIG

makewhatis: makewhatis.sh
	sed '/^MANDIR/,/^WHATIS/c\
MANDIR=$(USRMAN)\
WHATIS=$(LIBDIR)/whatis' $? > $@

phc:	phc.sh
	sed '/^NAMES/c\
NAMES=$(LIBDIR)/names' $? > $@

install: $(ALL)
	for i in $(SUBDIR); do cd $$i.d; make install; cd ..; done
	cp $(M0711) $(M0755) $(M4711) $(BINDIR)
	cd $(BINDIR); chmod 0711 $(M0711)
	cd $(BINDIR); chmod 0755 $(M0755)
	cd $(BINDIR); chmod 4711 $(M4711)
	cp $(M0700) $(ETC)
	cd $(ETC); chmod 0700 $(M0700)
	cp $(FORUUCP) $(LIBUUCP)
	cd $(LIBUUCP); chown uucp $(FORUUCP)
	cd $(LIBUUCP); chgrp uucp $(FORUUCP)
	cd $(LIBUUCP); chmod 4711 $(FORUUCP)


clean:
	for i in $(SUBDIR); do cd $$i.d; make clean; cd ..; done
	rm -f $(ALL)
