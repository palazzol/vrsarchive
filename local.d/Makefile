#
#	$Header: /home/Vince/cvs/local.d/Makefile,v 1.8 1988-01-17 19:59:44 vrs Exp $
#
MODEL	= -M2em -F 2000
OPTIONS	= -DMAP=\"$(LIBDIR)/uucp.map\"
TODO	= LaTeX.d dialers.d lbgm.d map.d news.d news.misc.d $(XENIX) $(M4711) \
	  dial.onet
XENIX	= cbuf cmap curse dial.onet mdemo smap top
SUBDIR	= arc ash arcmail calend calls car clear compress copymode crack ctc \
	  ctty cursive cxref diffc displays dithds elbat espel getopt hoc6 \
	  include index ispell jove kermit less lintlib mkman ofiles patch \
	  pathalias ptc read20 recipes roff se strings talk teco tmac tpr \
	  umodem uuencode vc vttest whatcomm which xmodem
M0700	= bootime esad logout mkbad
M0711	= bincmp clock from getfrom script
M0755	= backup datefudge dirsort dtgt dtree getto islide makewhatis \
	  phc ports slide vslide
M4711	= cbuf cmap # smap
ALL	= $(SUBDIR) $(M0700) $(M0711) $(M0755) $(M4711)
include ../CONFIG

makewhatis: makewhatis.sh
	sed '/^MANDIR/,/^WHATIS/c\
MANDIR=$(USRMAN)\
WHATIS=$(LIBDIR)/whatis' $? > $@

phc:	phc.sh
	sed '/^NAMES/c\
NAMES=$(LIBDIR)/names' $? > $@

.sh:
	cp $@.sh $@
	chmod 755 $@

install: $(ALL)
	for i in $(SUBDIR); do cd $$i.d; make install; cd ..; done
	cp $(M0711) $(M0755) $(M4711) $(BINDIR)
	cd $(BINDIR); chmod 0711 $(M0711)
	cd $(BINDIR); chmod 0755 $(M0755)
	cd $(BINDIR); chmod 4711 $(M4711)
	cp $(M0700) $(ETC)
	cd $(ETC); chmod 0700 $(M0700)

clean:
	for i in $(SUBDIR); do cd $$i.d; make clean; cd ..; done
	rm -f $(ALL)
