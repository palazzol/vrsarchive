#
#	%G% for uulock and dial.
#
all:	uulock dial

install: /usr/lib/uucp/uurmlock /usr/etc/dial

dial: dial.c
	cc -o dial dial.c

uulock: uulock.c
	cc -o uulock uulock.c

/usr/lib/uucp/uurmlock: uulock
	cp uulock /usr/lib/uucp/uurmlock
	chown uucp /usr/lib/uucp/uurmlock
	chgrp uucp /usr/lib/uucp/uurmlock
	chmod 6711 /usr/lib/uucp/uurmlock
	ln /usr/lib/uucp/uurmlock /usr/lib/uucp/uumklock

/usr/etc/dial: dial
	cp dial /usr/etc/dial
	chown root /usr/etc/dial
	chgrp root /usr/etc/dial
	chmod 6711 /usr/etc/dial

clean:
	rm -f *.o make.out uulock dial
