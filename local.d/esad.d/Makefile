#
#	$Header: /home/Vince/cvs/local.d/esad.d/Makefile,v 1.2 1990-03-30 17:02:48 vrs Exp $
#
ALL	= esad
MODEL	= -i -F 2000
include /etc/CONFIG

install: $(ALL) $(ETC) $(MAN8)
	cp $(ALL) $(ETC)
	cd $(ETC); chmod 700 $(ALL)
	cp esad.1 $(MAN8)
	cd $(MAN8); chmod 644 esad.1

clean:
	rm -f $(ALL)
