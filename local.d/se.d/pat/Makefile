# 
# $Header: /home/Vince/cvs/local.d/se.d/pat/Makefile,v 1.3 1987-02-13 10:07:58 vrs Exp $
# 
# $Log: not supported by cvs2svn $
# Version 1.2  87/02/07  20:31:12  vrs
# Need to include ../flags
# 
# Revision 1.2  86/07/11  15:25:14  osadr
# Changed to ensure that the proper 'ar' program gets used.
# 
# Revision 1.1  86/05/06  13:33:07  osadr
# Initial revision
# 
# 
# 
# makefile for pattern library for 'se'
ALL	= libpat.a
MODEL	= -M2l
OPTIONS = -O `cat ../flags`
include ../../../CONFIG

PR=pr

libpat.a: pat.o
	ar ruv libpat.a pat.o
	if test -x /usr/bin/ranlib; then /usr/bin/ranlib libpat.a; fi
	if test -x /bin/ranlib; then /bin/ranlib libpat.a; fi

install: ;

print:
	$(PR) pat.c Makefile | lpr -b 'pat lib'

print2: pat.c Makefile
	$(PR) $? | lpr -b 'new pat lib'

clean:
	rm -f pat.o $(ALL)
