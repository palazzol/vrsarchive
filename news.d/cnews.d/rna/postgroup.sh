# postgroups
#	Michael Rourke July 84
DCS= EE= STAFF= HON= PG=
for G in `/etc/bin/mkclass -l`
do
	case $G in
	DCS*|C-*)	DCS="$DCS $G";;
	EE*)	EE="$EE $G";;
	esac
	case $G in
	*Staff)	STAFF="$STAFF $G";;
	*HON)	HON="$HON $G";;
	*PG)	PG="$PG $G";;
	esac
done
ALL="$STAFF $HON $PG"
BAD= LIST=
if [ $# -gt 0 ]
then
	case "$1" in
	DCS|EE|STAFF|HON|PG|ALL) LIST="$LIST `eval echo '$'$1`";;
	*) echo "Unknown group: $1"
	   BAD=1;;
	esac
fi
if [ $# -eq 0 -o -n "$BAD" ]
then
	cat <<!
Usage: postgroup group [ postnews_args ... ]
    Groups are:
	STAFF	(All staff:$STAFF)
	DCS	(Dept of CS Staff, Hons & PG Students:
			$DCS)
	EE	(Elec Eng Staff, Hons & PG Students:$EE)
	HON	(Honours Students:$HON)
	PG	(Post Grad Students:$PG)
	ALL	(All the above:
			$ALL)
!
	exit 1
fi
NL=
for L in $LIST
do
	if [ ! "$NL" ]
	then
		NL="class.$L"
	else
		NL="$NL,class.$L"
	fi
done
exec /bin/postnews -n "$NL" $2*
