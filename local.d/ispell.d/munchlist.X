: Use /bin/sh
#
#	Given a list of words for ispell, generate a reduced list
#	in which all possible suffixes have been collapsed.  The reduced
#	list will match the same list as the original.
#
#	Usage:
#
#	munchlist [ -d hashfile ] [ -e ] [ -w chars ] [ file ] ...
#
#	Options:
#
#	-d hashfile
#		Remove any words that are covered by 'hashfile'.  The
#		default is the default ispell dictionary.  The words
#		will be removed only if all suffixes are covered by
#		the hash file.  A hashfile of /dev/null should be
#		specified when the main dictionary is being munched.
#	-e	Economical algorithm.  This will use much less temporary
#		disk space, at the expense of time.  Useful with large files
#		(such as complete dictionaries).
#	-w	Passed on to ispell (specify chars that are part of a word)
#
#	The given input files are merged, then processed by 'ispell -c'
#	to generate possible suffix lists;  these are then combined
#	and reduced.  The final result is written to standard output.
#
#	For portability to older systems, I have avoided getopt.
#
#		Geoff Kuenning
#		2/28/87
#
LIBDIR=/tmp2/lib			# Must match config.h
DEFDICT=dict.191			# Must match config.h
EXPAND1=${LIBDIR}/expand1.sed
EXPAND2=${LIBDIR}/expand2.sed
TDIR=${TMPDIR:-/usr/tmp}
TMP=${TDIR}/munch$$

cheap=no
dictopt=
wchars=
while [ $# != 0 ]
do
    case "$1" in
	-d)
	    case "$2" in
		/dev/null)
		    dictopt=NONE
		    ;;
		*)
		    dictopt="-d $2"
		    ;;
	    esac
	    shift
	    ;;
	-e)
	    cheap=yes
	    ;;
	-w)
	    wchars="-w $2"
	    shift
	    ;;
	*)
	    break
    esac
    shift
done
#
# Awk program to combine suffixes onto one line
#
AWKMUNCH='
    {
    if ($1 != old1  &&  old1 != "")
	{
	print old1 suffixes
	suffixes = ""
	}
    old1 = $1
    for (i = 2;  i <= NF;  i++)
	suffixes = suffixes "/" $i
    }
    END { if (old1 != "") print old1 suffixes }'
#
# Awk program to break suffixes up into one per line
#
AWKUNMUNCH='
    {
    print $1
    for (i = 2;  i <= NF;  i++)
	print $1 "/" $i
    }'
trap "/bin/rm -f ${TMP}*; exit 1" 1 2 15
#
# Collect all the input (cat), convert to uppercase (tr), expand all
# the suffix options (two sed's), and preserve (sorted) for later
# joining.  Unless an explicitly null dictionary was specified, remove
# all expanded words that are covered by the dictionary (ispell).
#
if [ "X$dictopt" = "XNONE" ]
then
    cat "$@" | tr '[a-z]' '[A-Z]' \
      | sed -f $EXPAND1 | sed -f $EXPAND2 | sort -u > ${TMP}a
else
    cat "$@" | tr '[a-z]' '[A-Z]' \
      | sed -f $EXPAND1 | sed -f $EXPAND2 | sort -u \
      | ispell -l $dictopt -p /dev/null > ${TMP}a
fi
#
# Munch the input to generate roots and suffixes (ispell -c).  We are
# only interested in words that have at least one suffix (egrep /);  the
# next step will pick up the rest.  Some of the roots are illegal.  We
# use join to restrict the output to those root words that are found
# in the original dictionary.
#
# Note:  one disadvantage of this pipeline is that for a large file,
# the join and awk may be sitting around for a long time while ispell
# and sort run.  You can get rid of this by splitting the pipe, at
# the expense of more temp file space.
#
if [ $cheap = yes ]
then
    ispell $wchars -c -d /dev/null -p /dev/null < ${TMP}a \
      | egrep / | sort -u -t/ +0 -1 +1 \
      | join -t/ - ${TMP}a | awk -F/ "$AWKMUNCH" > ${TMP}b
else
    ispell $wchars -c -d /dev/null -p /dev/null < ${TMP}a \
      | egrep / | sort -u -t/ +0 -1 +1 \
      | join -t/ - ${TMP}a > ${TMP}b
fi
#
# There is now one slight problem:  the suffix flags X, J, and Z
# are simply the addition of an "S" to the suffixes N, G, and R,
# respectively.  This produces redundant entries in the output file;
# for example, ABBREVIATE/N/X and ABBREVIATION/S.  We must get rid
# of the unnecessary duplicates.  The candidates are those words that
# have only an "S" flag (egrep).  We strip off the "S" (sed), and
# generate a list of roots that might have made these words (ispell -c).
# Of these roots, we select those that have the N, G, or R flags,
# replacing each with the plural equivalent X, J, or Z (sed -n).
# Using join once again, we select those that have legal roots
# and put them in ${TMP}c.
#
if [ $cheap = yes ]
then
    egrep '^[^/]*/S$' ${TMP}b | sed 's@/S$@@' \
      | ispell -c -d /dev/null -p /dev/null \
      | sed -n -e '/\/N/s/N$/X/p' -e '/\/G/s/G$/J/p' -e '/\/R/s/R$/Z/p' \
      | sort -u -t/ +0 -1 +1 \
      | join -t/ - ${TMP}a \
      | awk -F/ "$AWKMUNCH" > ${TMP}c
else
    egrep '^[^/]*/S$' ${TMP}b | sed 's@/S$@@' \
      | ispell -c -d /dev/null -p /dev/null \
      | sed -n -e '/\/N/s/N$/X/p' -e '/\/G/s/G$/J/p' -e '/\/R/s/R$/Z/p' \
      | sort -u -t/ +0 -1 +1 \
      | join -t/ - ${TMP}a > ${TMP}c
fi
#
# Now we have to eliminate the stuff covered by ${TMP}c from ${TMP}.
# First, we re-expand the suffixes we just made (sed -f pair), and let
# ispell re-create the /S version (ispell -c).  We select the /S versions
# only (egrep), sort them (sort) for comm, and use comm to delete these
# from ${TMP}b.  The output of comm (i.e., the trimmed version of
# ${TMP}b) is combined with our special-suffixes file ${TMP}c (sort,
# with preceding awk, if $cheap) and reduced in size (AWKMUNCH) to
# produce a final list of all words that have at least one suffix.
#
if [ $cheap = yes ]
then
    sed -f $EXPAND1 < ${TMP}c | sed -f $EXPAND2 \
      | ispell -c -d /dev/null -p /dev/null \
      | egrep '\/S$' | sort -u -t/ +0 -1 +1 | comm -13 - ${TMP}b \
      | awk -F/ "$AWKUNMUNCH" - ${TMP}c \
      | sort -u -t/ +0 -1 +1 - \
      | awk -F/ "$AWKMUNCH" > ${TMP}d
else
    sed -f $EXPAND1 < ${TMP}c | sed -f $EXPAND2 \
      | ispell -c -d /dev/null -p /dev/null \
      | egrep '\/S$' | sort -u -t/ +0 -1 +1 | comm -13 - ${TMP}b \
      | sort -u -t/ +0 -1 +1 - ${TMP}c \
      | awk -F/ "$AWKMUNCH" > ${TMP}d
fi
/bin/rm -f ${TMP}[bc]
#
# Now a slick trick.  Use ispell to select those (root) words from the original
# list (${TMP}a) that are not covered by the suffix list (${TMP}d).  Then we
# merge these with the suffix list and sort it to produce the final output.
#
ispell $wchars -d /dev/null -p ${TMP}d -l < ${TMP}a | tr -d \\015 \
  | sort -u -t/ +0 -1 +1 - ${TMP}d
/bin/rm -f ${TMP}*
