#
#	$Header: /home/Vince/cvs/local.d/pathalias.d/makefile,v 1.3 1987-12-26 20:17:19 vrs Exp $
#
XENIX	= makedb
ALL	= pathalias
MODEL	= -Ml
OPTIONS	= -DALIASDB=\"$(LIBDIR)/palias\"
include ../../CONFIG
# pathalias -- by steve bellovin, as told to peter honeyman
# if you can't or don't intend to use dbm files, don't bother with makedb
DBM	= -ldbm
# or if you roll your own ...
# DBM	= dbm.o
YFLAGS	= -d
OBJ	= addlink.o addnode.o extern.o local.o main.o mapit.o mapaux.o mem.o \
	  parse.o yylex.o
HDRS	= def.h config.h
SRC	= addlink.c addnode.c extern.c local.c main.c mapit.c mapaux.c mem.c \
	  parse.y yylex.c
CSRC	= addlink.c addnode.c extern.c local.c main.c mapit.c mapaux.c mem.c \
	  parse.c yylex.c

pathalias: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBE) -o $@

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp pathalias.1 $(MAN1)
	cd $(MAN1); chmod 644 pathalias.1

$(OBJ):	def.h

# if touch had a proper exit status, this would work...
#	touch def.h || get -s sccs/s.def.h
def.h: config.h
	if test -f def.h; then touch def.h; else get -s sccs/s.def.h; fi

parse.c: parse.y def.h
	$(YACC) $(YFLAGS) parse.y
	mv y.tab.c parse.c

yylex.o:	parse.c yylex.c

makedb: makedb.o
	$(CC) $(CFLAGS) makedb.o $(DBM) -o $@

makedb.o: config.h

clean:
	rm -f *.o $(ALL) y.tab.c y.tab.h parse.c core

tags: $(SRC) $(HDRS) makedb.c
	ctags -w $(HDRS) $(SRC)

bundle:
	@bundle README CHANGES pathalias.1 Makefile ${HDRS} ${SRC} makedb.c

lint:	$(CSRC)
	lint $(CFLAGS) $(CSRC)
	lint makedb.c


# the remainder is site specific and can be deleted.

# administering paths on 6 machines is easy -- here's how i do it

# hosts running delivermail
DMEQUIV	= tilt

# hosts running sendmail
SMEQUIV	= quirky flakey yoyo

# all neighbors
NEIGHBORS = ${DMEQUIV} ${SMEQUIV} princeton

# including me
SITES	= down ${NEIGHBORS}

# in v8, * includes . files, sh has extended syntax.
PATHFILES = paths/[^.]* paths.bell/[^.]* path.map/[^.]*

# from observation and rumor
# avoid like the plague
DEADHOSTS = -d harpo -d zeppo -d chico -d gummo -d tucc -d hogpc -d eosp1 -d twg

DEADLINKS = -d fortune!analog -d uiucdcs!uokmet -d siemens!uiucdcs -d vax135!uw-beaver -d research!eagle -d decvax!humus -d ulysses!ucbarpa

# nets that require gateways
DEADNETS = -d CSNET -d BITNET -d ARPA -d DEC -d MAILNET

DEAD	= ${DEADHOSTS} ${DEADLINKS} ${DEADNETS}

# map output to lower case.  dead links.
ARGS	= -i $(DEAD)

paths:	${SITES}

# don't touch "down" until completely done.
down:	paths/princeton
	pathalias -v ${ARGS} $(PATHFILES) > down.new 2>ERRORS
	sort down.new > down
#	rm down.new

# NEIGHBORS have exactly the same links as down, so turn
#	down	%s
#	up	up!%s
# into
#	up	%s
#	down	down!%s

# generate phoney costs for delivermail neighbors
${DMEQUIV}:	down
	sed -e "s/^down	%s$$/$@	%s/" -e "s/^$@	$@!%s$$/down	down!%s/" -e 's/^/0	/' down > $@
	
# reorder the output and generate phoney costs for sendmail neighbors
${SMEQUIV}: down
	sed -e "s/^down	%s$$/$@	%s/" -e "s/$@	$@!%s$$/down	down!%s/" -e 's/\(.*\)	\(.*\)/\1	0	\2/' down > $@

# ship it!
sendit: ${NEIGHBORS}
	for i in ${DMEQUIV}; do\
		uucp -ga $$i $$i!/usr/local/lib/pathaliases;\
		uux -z -gb $$i!newaliases;\
	done
	for i in ${SMEQUIV} princeton; do uucp $$i $$i!~/paths; done
	touch sendit

# reorder the output for princeton/sendmail/uubang and generate phoney costs.
princeton: down
	pathalias ${ARGS} -l $@ $(PATHFILES)> $@.new 2>ERRORS
	sed -e 's/\(.*\)	\(.*\)/\1	0	\2/' $@.new > $@
	rm $@.new

# make output for friends.  (make friends for output?)
sfyog rcalabs neoucom:
	pathalias ${ARGS} -l $@ $(PATHFILES) > $@ 2>ERRORS

# desperation debugging -- examine the costs.
costs:
	pathalias -vci ${DEAD} -l down $(PATHFILES) > down.costs 2>ERRORS

# make one BIG file.  a bad idea.
cat:
	cat $(PATHFILES) > CAT

# generate test data using undocumented features.  (go away.)
edges: down
	pathalias -i -g edges $(PATHFILES)

