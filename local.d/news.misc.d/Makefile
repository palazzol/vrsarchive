#
#	$Header: /home/Vince/cvs/local.d/news.misc.d/Makefile,v 1.1 1987-02-13 11:31:01 vrs Exp $
#
MODEL	= -M2em -F 2000
SUBDIR	= ngstats rmgroup uuhosts
ALL	= $(SUBDIR)
include ../../CONFIG

install: $(ALL)
	for i in $(SUBDIR); do cd $$i.d; make install; done

clean:
	for i in $(SUBDIR); do cd $$i.d; make clean; cd ..; done
	rm -f $(ALL)
