: /bin/sh
# '@(#) uuhosts.sh 1.39 84/12/15'

# PATH will have to be adjusted for non-BSD systems.
PATH=/usr/local:/usr/ucb:/bin:/usr/bin
LIB=/usr/local/lib
NEWS=$LIB/news
MAPS=$NEWS/maps
NEWSMAPGROUP=mod.map.news
UUCPMAPGROUP=mod.map.uucp
NEWSMAP=$NEWSMAPGROUP
UUCPMAP=$UUCPMAPGROUP
MAPSH=$NEWS/mapsh

# Routing information produced by pathalias.
paths=$LIB/nmail.paths

# The directories $MAPS/$NEWSMAP and $MAPS/$UUCPMAP contain the map information
# extracted from the newsgroups mod.map.news (for the USENET news map)
# and mod.map.uucp (for the UUCP mail map).  The extraction is done by
# a line in $NEWS/sys like this:

# maps:mod.map.news,mod.map.mail:B:/usr/local/uuhosts -x

# Locally-known USENET news map information should go in $MAPS/$NEWSMAP/Local.

# $MAPSH is needed, to use the chroot(2) system call to limit
# what can be done when executing a shell with a news article as input.
# $MAPS, $MAPS/bin and $MAPS/bin/* must be unwritable by anyone:

# $MAPS: total 16
# dr-xr-xr-x  6 root     news          512 Nov 11 16:42 .
# drwxrwxr-x 19 news     news         1024 Nov 11 16:45 ..
# dr-xr-xr-x  2 root     news          512 Nov 11 16:39 bin
# drwxrwxr-x  2 news     news         2048 Nov 11 16:42 mod.map.news
# drwxrwxr-x  2 news     news        10240 Nov 11 16:39 mod.map.uucp
# drwxrwxrwx  2 news     news           24 Nov 11 16:41 tmp

# $MAPS/bin: total 59
# -r-xr-xr-x  1 root     news        10240 Nov 11 15:29 cat
# -r-xr-xr-x  1 root     news         4096 Nov 11 16:33 echo
# -r-xr-xr-x  1 root     news        18432 Nov 11 15:29 sed
# -r-xr-xr-x  1 root     news        27648 Nov 11 15:29 sh

# Update notices are mailed to postmaster (for UUCP) and usenet (for news),
# which should be aliases which redistribute to the local mail and news
# (respectively) administrators.

cd $NEWS

case $1 in
	-x)
		# extract a new map piece into a map directory
		temphead=/tmp/maphead.$$
		temptext=/tmp/maptext.$$
		tempcomm=/tmp/mapcomm.$$
		cp /dev/null $temphead
		cp /dev/null $temptext
		echo 'exec /bin/mail usenet' > $tempcomm
		awk '
BEGIN	{
	temphead = "'$temphead'"; tempcomm = "'$tempcomm'";
	typeset = 0; isnewsmap = 0; isuucpmap = 0;
	shead = 0; stext = 1; snews = 2; suucp = 3;
	state = shead;
}
state == shead && ($1 == "From:" || $1 == "Sender:" \
    || $1 == "Date:" || $1 == "Message-ID:" || $1 == "Message-Id:") {
	print "Original-" $0 >> temphead;
	next;
}
state == shead && $1 == "Newsgroups:" {	# no cross-postings allowed
	if ($2 == "'$NEWSMAPGROUP'") {
		isnewsmap = 1;
		typeset = 1;
		print "Reply-To: usenet" >> temphead;
		print "exec /bin/mail usenet" > tempcomm;
	} else if ($2 == "'$UUCPMAPGROUP'") {
		isuucpmap = 1;
		typeset = 1;
		print "Reply-To: postmaster" >> temphead;
		print "exec /bin/mail postmaster" > tempcomm;
	}
}
state == shead && $1 == "Subject:" {
	if ($1 == "Re:" || $1 == "RE:" || $1 == "re:"\
	|| !typeset) {	# this requires Newsgroups: before Subject:
		print "Subject:  not a map update" >> temphead;
		print "Original-" $0 >> temphead;
	} else
		print $0 >> temphead;
	next;
}
state == shead && /^$/	{
	if (isnewsmap != 0) {
		print "cd '$NEWSMAP'" | "uuhosts -n";
		state = snews;
	} else if (isuucpmap != 0) {
		print "cd '$UUCPMAP'" | "uuhosts -u";
		state = suucp;
	} else
		state = stext;
}
state == shead {
		print $0 >> temphead;
}
state == snews	{
	print | "uuhosts -n";
}
state == suucp	{
	print | "uuhosts -u";
}
state == stext	{
	print;
}
' > $temptext 2>&1
		cat $temphead $temptext | sh $tempcomm
		rm -f $temphead $temptext $tempcomm
		exit 0
	;;

	-u)
		# extract a UUCP map piece
		$MAPSH
		cd $MAPS/$UUCPMAP
		for f in *.a *.ar
		do
			ar xv $f
			rm $f
		done
		exit 0
	;;

	-n)
		# extract a USENET map piece
		$MAPSH
		exec uuhosts -i
		exit 0
	;;

	-i)
		# make an index for the USENET map
		cd $MAPS/$NEWSMAP
		awk '$1 == "Name:" { 
			printf ("%s\t%s\n", $2, FILENAME);
		}' Local [a-z]* | sort -f > Index.$$
		mv Index.$$ Index
		exit 0
	;;

	-g)
		# by geographical region
		cd $MAPS/$NEWSMAP
		shift
		if test $# -eq 0
		then
			exec ls
			exit 1
		fi
		exec cat $*
		exit 1
	;;

	-k)
		# by keyword
		cd $MAPS/$NEWSMAP
		shift
		exec awk '
BEGIN		{ inside = 1; outside = 0; state = outside; }
/^Name:/	{ state = inside; count = 0; useit = 0; }
state == inside	{ block[count++] = $0; }
/'"$*"'/	{ useit = 1; }
/^$/ && state == inside	{
	if (useit == 1) {
		for (i = 0; i < count; i++) {
			print block[i];
		}
	}
	state = outside;
}
' *
		exit 1
	;;

	-*)
		# unknown option
	;;

	"")
		# no arguments
	;;

	*)
		# by site name
		if [ -x /usr/bin/look ]; then
			look=/usr/bin/look
			lookopt="-f "
		else
			look=grep
			lookopt="^"
		fi
		for arg in $*
		do
			echo 'UUCP mail path:'
			$look $lookopt$arg $paths
			echo '
UUCP mail host information:'
			cd $MAPS/$UUCPMAP
			sed -e '
s/^#N/#Name		/
s/^#S/#System-CPU-OS	/
s/^#O/#Organization	/
s/^#C/#Contact	/
s/^#E/#Electronic-Address/
s/^#T/#Telephone	/
s/^#P/#Postal-Address	/
s/^#L/#Latitude-Longitude/
s/^#R/#Remarks	/
s/^#W/#Written-by	/' ${arg}*
			cd $MAPS/$NEWSMAP
			echo '
USENET news host information:'
			sed -n -e "/^Name:[ 	]*${arg}/,/^$/p" \
				`$look $lookopt$arg Index | awk '{print $2}'`
		done
		exit 0
	;;
esac

echo 'Usage:	'uuhosts' hostname ...
for information about a particular UUCP or USENET host or hosts, or

	'uuhosts' -g geographical-region
for information about USENET news sites in a geographical region, or

	'uuhosts' -g
for a list of known USENET geographical-regions.

See uuhosts(1) for further details and more obscure options.
'
exit 1
