: '@(#)install.sh	1.5	9/3/84'

if test "$#" != 5
then
	echo "usage: $0 spooldir libdir nuser ngroup ostype"
	exit 1
fi
SPOOLDIR=$1
LIBDIR=$2
NEWSUSR=$3
NEWSGRP=$4
OSTYPE=$5

: Get name of local system
case $OSTYPE in
	usg)	SYSNAME=`uname -n`
		case $SYSNAME in
			'(empty)')	SYSNAME=`uuname -l`;;
			*)		;;
		esac;;
	usg)	SYSNAME=`uname -n`;;
	v7)	SYSNAME=`uuname -l`
		touch $LIBDIR/history.pag $LIBDIR/history.dir;;
	*)	echo "$0: Unknown Ostype"
		exit 1;;
esac

if test "$SYSNAME" = ""
then
	echo "$0: Cannot get system name"
	exit 1
fi

: Ensure SPOOLDIR exists
if test ! -d $SPOOLDIR
then
	mkdir $SPOOLDIR
fi
chmod 777 $SPOOLDIR
chown $NEWSUSR $SPOOLDIR
chgrp $NEWSGRP $SPOOLDIR

chown $NEWSUSR $LIBDIR
chgrp $NEWSGRP $LIBDIR

: Ensure certain files in LIBDIR exist
touch $LIBDIR/history $LIBDIR/active $LIBDIR/log $LIBDIR/errlog $LIBDIR/users
chmod 666 $LIBDIR/users

: If no sys file, make one.
if test ! -f $LIBDIR/sys
then
echo
echo Making a $LIBDIR/sys file to link you to oopsvax.
echo You must change oopsvax to your news feed.
echo If you are not in the USA, remove '"usa"' from your line in the sys file.
echo If you are not in North America, remove '"na"' from your line in the sys file.
	cat > $LIBDIR/sys << EOF
$SYSNAME:net,fa,mod,na,usa,to::
oopsvax:net,fa,mod,na,usa,to.oopsvax::
EOF
fi

: If no seq file, make one.
if test ! -s $LIBDIR/seq
then
	echo '100' >$LIBDIR/seq
fi

: if no moderators file, make one.
if test ! -f $LIBDIR/moderators
then
	cat > $LIBDIR/moderators << EOF
net.announce	cbosgd!mark
mod.ber		bellcore!ber
EOF
echo
echo Make sure the uucp paths in $LIBDIR/moderators are correct for your site.
fi

sh makeactive.sh $LIBDIR $SPOOLDIR $NEWSUSR $NEWSGRP

for i in $LIBDIR/ngfile $BINDIR/inews
do
	if test -f $i
	then
		echo "$i is no longer used. You should remove it."
	fi
done

: if no aliases file, make one
if test ! -f $LIBDIR/aliases
then
	cat >$LIBDIR/aliases <<EOF
net.trivia	net.games.trivia
net.stat	net.math.stat
net.unix.wizards	net.unix-wizards
net.apollo	net.works.apollo
net.puzzles	net.puzzle
EOF
fi

: if no distributions file, make one
if test ! -f $LIBDIR/distributions
then
	cat >$LIBDIR/distributions <<EOF
local		Local to this site
usa		Everywhere in the USA
na		Everywhere in North America
world		Everywhere on Usenet in the world (same as net)
EOF
echo
echo You may want to add distributions to $LIBDIR/distributions if your
echo site particpates in a regional distribution such as '"ba"' or '"dc"'.
fi

chown $NEWSUSR $LIBDIR/[a-z]*
chgrp $NEWSGRP $LIBDIR/[a-z]*

echo
echo Reminder: uux must permit rnews if running over uucp.
