HERSHDIR=%HERSHDIR%
FONTDIR=%FONTDIR%

HERSHFILES=$HERSHDIR/hersh.oc[1234]
HMPFILE=$HERSHDIR/$1.hmp
FNTFILE=$FONTDIR/$1.fnt
FNTLINES=96

if [ ! -f $HMPFILE ]
then
	echo "$HMPFILE does not exist.  Usage: mk_font <fontname>";
	exit 1;
fi

# filter the hershey database to merge
# character descriptions into single lines

trap "rm -f /tmp/hersh.$$;exit 1" 0 1 2 15

cat $HERSHFILES | awk '{
	if (substr ($0, 1, 1) == " ") {
		catnum = substr ($0, 1, 5);
		if (catnum+0 != 0) {
			printf ("\n");
		}
	}
	printf ("%s", $0);
}' > /tmp/hersh.$$
	
# now grep for each entry in the hmp file.
# the awk script simply expands entries in the hmp file
# of the form "2551-2576" into the range of numbers indicated.

awk '{
	for (i = 1; i <= NF; i++) {
		n = split ($i, a, "-");

		if (n == 1) {
			print $i;
		} else {
			for (j = a[1]; j <= a[2]; j++) {
				print j;
			}
		}
	}
}' < $HMPFILE | xargs -i egrep "^ *{}" /tmp/hersh.$$ > $FNTFILE

# superficially check the resulting .fnt file for validity

fntlines=`wc -l $FNTFILE | awk '{print $1}'`
if [ $fntlines -ne $FNTLINES ]
then
	echo "$FNTFILE has only $fntlines lines... should have $FNTLINES";
	echo "It is likely that one of the following files is corrupted:";
	echo "";
	echo $HERSHFILES $HMPFILE;
	echo "";
	echo "In any case, $FNTFILE is not a valid database for hbanner.";
	exit 1
fi
