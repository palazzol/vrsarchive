: check active file for missing or extra newsgroups
: '@(#)checkgroups	1.4	9/4/84'

cp LIBDIR/localgroups LIBDIR/newsgroups
cat >>LIBDIR/newsgroups
echo junk >/tmp/$$a
echo control >>/tmp/$$a
sed 's/[ \	].*//' LIBDIR/newsgroups |
egrep "^net.|^fa.|^general" >>/tmp/$$a
sort -u /tmp/$$a -o /tmp/$$a
egrep "^net.|^fa.|^general|^junk|^control" LIBDIR/active | sed 's/ .*//' | sort  -u >/tmp/$$b

comm -13 /tmp/$$a /tmp/$$b >/tmp/$$remove
comm -23 /tmp/$$a /tmp/$$b >/tmp/$$add

if test -s /tmp/$$remove
then
	(
	echo "The following newsgroups are not valid and should be removed."
	sed "s/^/	/" /tmp/$$remove
	echo ""
	echo "You can do this by executing the command:"
	echo \	LIBDIR/rmgroup `cat /tmp/$$remove`
	echo ""
	) 2>&1 >/tmp/$$out
fi

if test -s /tmp/$$add
then
	(
	echo "The following newsgroups were missing and were added."
	sed "s/^/	/" /tmp/$$add
	echo ""
	for i in `cat /tmp/$$add`
	do
		LIBDIR/inews -n control -t "cmsg newgroup $i" </dev/null
	done
	) 2>&1 >>/tmp/$$out
fi

if test -s /tmp/$$out
then
	(echo	"Subject: Problems with your active file"
	echo ""
	cat /tmp/$$out
	) | if test $# -gt 0
		then
			mail $1
		else
			cat
		fi
fi

rm -f /tmp/$$*
