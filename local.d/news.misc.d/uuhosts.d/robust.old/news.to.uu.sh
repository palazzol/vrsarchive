: /bin/sh
#
#	This command, uucp.from.news, is a stopgap measure for use until
#	the UUCP map is complete.  It extracts mail information from the
#	USENET news map and uses it to fill in gaps in the UUCP mail map.
#
#	It should be called from the news map directory, $MAPS/$NEWSMAP,
#	with a list of news map files as arguments, i.e.:
#		cd $MAPS/$NEWSMAP
#		uucp.from.news *
#	It looks through the news map entries for ones with mail information,
#	extracts those into individual files in $DIR by host name, and later
#	copies all those for which there are not already host entries in
#	the mail map directory to the mail map directory,  $MAPS/UUCPMAP.
#	The new mail map files all have '-' appended so they can be easily
#	distinguished.  If uucp.from.news is run again, such
#	$MAPS/UUCPMAP/*- entries will be overwritten.
#
#	The temporary directory $DIR is removed after the command is done.
#
PATH=/usr/local:/usr/ucb:/bin:/usr/bin
umask 0002

LIB=/usr/local/lib
NEWS=$LIB/news
MAPS=$NEWS/maps
NEWSMAPGROUP=mod.map.news
UUCPMAPGROUP=mod.map.uucp
NEWSMAP=$NEWSMAPGROUP
UUCPMAP=$UUCPMAPGROUP

DIR=uucptmp
mkdir $DIR

awk '
BEGIN {
	sbetween = 0; sinside = 1; state = sbetween;
	Date = "'"`date`"'";
	split (Date, date, " ");
	Mark = "uucp.from.news " date[3] " " date[2] " " date[6];
	dir = "'$DIR'";
}
state == sbetween && $1 == "Name:" {
	state = sinside;
	last = "";
	Name = "";
	System = "";
	Organization = "";
	Contact = "";
	Telephone = "";
	Postal = "";
	Address = "";
	News = "";
	Mail = "";
	Latlong = "";
	Remarks = "";
	Written = "";
	Comments = "";
}
state != sinside {
	next;
}
# { print $0; }
$1 == "Name:"		{ Name = $2;		next; }
$1 == "Organization:"	{
	Organization = $2;
	for (x = 3; x <= NF; x++)
		Organization = Organization " " $x;
	last = $1;
	next;
}
$1 == "Contact:"	{
	Contact = $2;
	for (x = 3; x <= NF; x++)
		Contact = Contact " " $x;
	last = $1;
	next;
}
$1 == "Phone:"		{
	Telephone = $2;
	for (x = 3; x <= NF; x++)
		Telephone = Telephone " " $x;
	last = $1;
	next;
}
$1 == "Postal-Address:"	{
	Postal = $2;
	for (x = 3; x <= NF; x++)
		Postal = Postal " " $x;
	last = $1;
	next;
}
$1 == "Electronic-Address:" {
	Address = $2;
	for (x = 3; x <= NF; x++)
		Address = Address " " $x;
	last = $1;
	next;
}
$1 == "News:"		{
	News = $2;
	for (x = 3; x <= NF; x++)
		News = News " " $x;
	last = $1;
	next;
}
/^	/ && last == "News:" {
	for (x = 2; x <= NF; x++)
		News = News " " $x;
	next;
}
$1 == "Mail:"		{
	Mail = $2;
	for (x = 3; x <= NF; x++)
		Mail = Mail " " $x;
	last = $1;
	next;
}
/^	/ && last == "Mail:" {
	for (x = 1; x <= NF; x++)
		Mail = Mail " " $x;
	next;
}
$1 == "Comments:"	{
	last = $1;
	if ($2 == "last" && $3 == "edited") {
		Written = $4;
		for (x = 5; x <= NF; x++)
			Written = Written " " $x;
		next;
	}
	Comments = $2;
	for (x = 3; x <= NF; x++)
		Comments = Comments " " $x;
	next;
}
/^$/ {
	state = sbetween;
	if (Mail == "")
		next;
	output = dir "/" Name;
	printf ("echo x - %s\n", output);
	printf ("cat > %s << '\''End-of-%s'\''\n", output, output);
	printf ("#N\t%s\n", Name);
	printf ("#S\t%s\n", System);
	printf ("#O\t%s\n", Organization);
	printf ("#C\t%s\n", Contact);
	printf ("#E\t%s\n", Address);
	printf ("#T\t%s\n", Telephone);
	printf ("#P\t%s\n", Postal);
	printf ("#L\t%s\n", Latlong);
	printf ("#R\t%s\n", Mark);
#	split (News, news, "");
#	printf ("#R\tNews:  %s", news[1]);
#	for (x = 2; news[x] != ""; x++) {
#		if ((x % 8) == 0)
#			printf ("\n\t");
#		else
#			printf (", ");
#		printf ("%s", news[x]);
#	}
#	printf ("\n");
	printf ("#W\t%s\n", Written);
	printf ("#\n");
	split (Mail, mail, " ");
	printf ("%s\t%s", Name, mail[1]);
	for (x = 2; mail[x] != ""; x++) {
		if ((x % 8) == 0)
			printf ("\n\t");
		else
			printf (", ");
		printf ("%s", mail[x]);
	}
	printf ("\n");
	if (Comments != "") {
		printf ("#\n");
		printf ("#%s\n", Comments);
	}
	printf ("\n");
	printf ("End-of-%s\n", output);
	next;
}
' $* | sh

cd $DIR
for f in *
do
	there=$MAPS/$UUCPMAP/$f
	if [ -r ${there} ]; then
		echo ${there}
		continue
	fi
	if [ -r ${there}. ]; then
		echo ${there}.
		continue
	fi
	if [ -r ${there}% ]; then
		echo ${there}%
		continue
	fi
#	if [ -r ${there}- ]; then
#		echo ${there}-
#		continue
#	fi
	echo new ${there}-
	mv $f ${there}-
done
cd ..
rm -rf $DIR
