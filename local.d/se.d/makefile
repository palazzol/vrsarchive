# 
# $Header: /home/Vince/cvs/local.d/se.d/makefile,v 1.6 1987-12-26 20:57:23 vrs Exp $
# 
DOCS= Makefile README
MANS= scriptse.1 se.1
BINS= se scriptse
ALL	= $(BINS) $(MANS)
MODEL	= -M2l
OPTIONS	= -O `cat flags`
include ../../CONFIG
#
# $Log: not supported by cvs2svn $
# Version 1.5  87/02/15  20:42:07  vrs
# Need to make clean in se_h too
# 
# Version 1.4  87/02/15  20:38:29  vrs
# Make 'make clean' consistent with other Makefiles
# 
# Version 1.3  87/02/15  15:59:08  vrs
# Use CONFIG
# 
# Version 1.2  87/02/07  20:42:33  vrs
# Need flags when linking
# 
# Revision 1.3  86/09/19  12:16:19  arnold
# Fixed to ignore return code from shell if statement.
# 
# Revision 1.2  86/05/27  17:47:50  osadr
# Changes to support the Unix PC (no egrep, shared library), and
# to support making if . is not in the search path.
# 
# Revision 1.1  86/05/06  13:39:18  osadr
# Initial revision
# 
# 
# 
# makefile for the Georgia Tech Screen Editor, 'se'

HEADERS= ascii.h constdefs.h extern.h se.h

SRCS= docmd1.c docmd2.c edit.c main.c misc.c scratch.c screen.c term.c
OBJS= docmd1.o docmd2.o edit.o main.o misc.o scratch.o screen.o term.o

LIBRARIES= libchangetty/libchangetty.a pat/libpat.a

###########################################################################
# Begin system dependant macro definitions

# PR is to print the files nicely.  Use pr -n if available, or else just pr
# I use a private utility called 'prt'
PR=pr

# INSTALL is the program to do the installation, use cp for real work
INSTALL	= cp

# CHOWN changes the owner.
CHOWN	= chown

# CHGRP changes the group.
CHGRP	= chgrp

# CHMOD will change permissions.
CHMOD	= chmod

########
# other things to change:
#
# on non-BSD systems, change the 'lpr' below to 'lp'
########

# Begin list of dependencies

se: $(OBJS) $(LIBRARIES)
	if grep DBSD flags > /dev/null || grep US5R2 flags > /dev/null; \
	then	echo -ltermlib > libs; \
	else	echo -lcurses > libs ; \
	fi
	-if [ -f /lib/shlib.ifile ] ; \
	then	ld /lib/crt0s.o /lib/shlib.ifile $(OBJS) $(LIBRARIES) -o $@ ; \
	else	$(CC) `cat flags` $(CFLAGS) $(OBJS) $(LIBRARIES) `cat libs` -o $@ ; \
	fi
	rm libs

$(OBJS): $(HEADERS) flags

flags: where
	./where > flags

libchangetty/libchangetty.a: libchangetty/changetty.c
	cd libchangetty; make

pat/libpat.a: pat/pat.c
	cd pat; make

scriptse: scriptse.c
	$(CC) -O scriptse.c -o scriptse

se.1: se.m4 flags
	(./m4munge $(CFLAGS) ; cat se.m4) | m4 | sed '/^$$/d' > se.1

install: $(ALL) $(BINDIR) $(MANS)
	$(INSTALL) $(BINS) $(BINDIR)
	cd $(BINDIR); $(CHMOD) 711 $(BINS) 
	$(INSTALL) $(MANS) $(MAN1)
	cd $(MAN1); $(CHMOD) 644 $(MANS)
	cd se_h; make install
	
print:
	$(PR) $(HEADERS) $(SRCS) $(DOCS) $(MANS) | lpr

printman: $(MANS)
	$(NROFF) -man $(MANS) | col | lpr

print2:	$(HEADERS) $(SRCS) $(DOCS) $(MANS)
	$(PR) $? | lpr

printall: printman print
	cd pat; make print
	cd libchangetty; make print
	cd se_h; make print

clean:
	rm -f $(OBJS) flags se.1 $(BINS)
	cd se_h; make clean
	cd pat; make clean
	cd libchangetty; make clean
