#
#	$Header: /home/Vince/cvs/local.d/esad.d/Makefile,v 1.3 2002-11-23 07:49:06 Vincent Exp $
#
ALL	= esad
MODEL	= -i -F 2000
include $(HOME)/CONFIG

install: $(ALL) $(ETC) $(MAN8)
	cp $(ALL) $(ETC)
	cd $(ETC); chmod 700 $(ALL)
	cp esad.1 $(MAN8)
	cd $(MAN8); chmod 644 esad.1

clean:
	rm -f $(ALL)
