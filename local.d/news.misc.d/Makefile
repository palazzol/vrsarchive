#
#	$Header: /home/Vince/cvs/local.d/news.misc.d/Makefile,v 1.3 2002-11-23 07:51:54 Vincent Exp $
#
MODEL	= -M2em -F 2000
SUBDIR	= ngstats rmgroup uuhosts
ALL	= $(SUBDIR)
include $(HOME)/CONFIG

install: $(ALL)
	for i in $(SUBDIR); do cd $$i.d; make install; done

clean:
	for i in $(SUBDIR); do cd $$i.d; make clean; cd ..; done
	rm -f $(ALL)
