#
#	$Header: /home/Vince/cvs/lib.d/Makefile,v 1.2 1990-04-13 15:06:22 vrs Exp $
#
TODO	= plot
ALL	= berkeley dir dpy fft
include /etc/CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done
