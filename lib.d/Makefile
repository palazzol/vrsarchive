#
#	$Header: /home/Vince/cvs/lib.d/Makefile,v 1.1 1987-02-16 11:20:05 vrs Exp $
#
TODO	= plot
ALL	= berkeley dir dpy fft
include ../CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done
