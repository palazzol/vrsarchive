#
#	$Header: /home/Vince/cvs/local.d/news.misc.d/Makefile,v 1.2 1990-03-30 17:03:10 vrs Exp $
#
MODEL	= -M2em -F 2000
SUBDIR	= ngstats rmgroup uuhosts
ALL	= $(SUBDIR)
include /etc/CONFIG

install: $(ALL)
	for i in $(SUBDIR); do cd $$i.d; make install; done

clean:
	for i in $(SUBDIR); do cd $$i.d; make clean; cd ..; done
	rm -f $(ALL)
