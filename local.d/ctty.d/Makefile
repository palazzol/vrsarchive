CFLAGS	= -Me

all:	ttype tspeed

ttype:	ctty.o
	cc $(CFLAGS) -F 2000 -o ttype ctty.o -ltermlib
	chown root ttype
	chmod 4711 ttype

tspeed:	ttype
	ln ttype tspeed
