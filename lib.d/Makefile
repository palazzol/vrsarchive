#
#	$Header: /home/Vince/cvs/lib.d/Makefile,v 1.3 2002-11-23 07:45:50 Vincent Exp $
#
TODO	= plot
#ALL	= berkeley dir dpy fft
ALL	= dir dpy fft
include $(HOME)/CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done
