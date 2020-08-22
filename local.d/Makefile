#
#	$Header: /home/Vince/cvs/local.d/Makefile,v 1.21 2002-11-26 23:27:37 vrs Exp $
#
MODEL	= -M2em -F 2000
OPTIONS	= -DMAP=\"$(LIBDIR)/uucp.map\"
TODO	= LaTeX.d ctty.d dialers.d lbgm.d map.d news.d news.misc.d $(XENIX) \
	  bootime logout$(EXE) mkbad$(EXE) $(M0700) smap$(EXE) dial.onet$(EXE) \
	  ofiles se
XENIX	= cbuf cmap curse dial.onet$(EXE) mdemo smap$(EXE) top
SUBDIR	= arc ash arcmail calend calls car clear compress copymode crack ctc \
	  cursive cxref diffc displays dithds elbat espel getopt hoc6 \
	  include index ispell jove kermit less lintlib mkman msdos \
	  patch pathalias ptc read20 recipes roff se strings sucko talk teco \
	  tmac tpr uemacs umodem uuencode vc vttest whatcomm which xlisp \
	  xmodem zmodem
M0700	= esad #bootime logout$(EXE) mkbad$(EXE)
M0711	= bincmp$(EXE) clock$(EXE) from$(EXE) getfrom$(EXE) script$(EXE)
M0755	= backup datefudge dirsort dtgt dtree getto islide makewhatis \
	  phc ports slide vslide
M4711	= cbuf$(EXE) cmap$(EXE) # smap$(EXE)
ALL	= $(SUBDIR) $(M0700) $(M0711) $(M0755) $(M4711)
include $(HOME)/CONFIG

makewhatis: makewhatis.sh
	echo '#!/bin/sh' >$@+
	echo MANDIR=$(USRMAN) >>$@+
	echo WHATIS=$(LIBDIR)/whatis >>$@+
	cat $? >> $@+
	mv $@+ $@

phc:	phc.sh
	echo '#!/bin/sh' >$@+
	echo NAMES=$(LIBDIR)/names >>$@+
	cat $? >> $@+
	mv $@+ $@

.sh:
	cp $@.sh $@
	chmod 755 $@

install: $(ALL) $(BINDIR) $(ETC)
	for i in $(SUBDIR); do (cd $$i.d && make install); done
	cp $(M0711) $(M0755) $(M4711) $(BINDIR)
	cd $(BINDIR); chmod 0711 $(M0711)
	cd $(BINDIR); chmod 0755 $(M0755)
	cd $(BINDIR); chmod 4711 $(M4711)
	cp $(M0700) $(ETC)
	cd $(ETC); chmod 0700 $(M0700)

clean:
	for i in $(SUBDIR); do (cd $$i.d && make clean); done
	rm -f *.o $(ALL)
