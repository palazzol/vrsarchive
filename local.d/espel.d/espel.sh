: "*
   *	espel - an enhanced spelling helper
   *		written by Eric Grimm
   *"
LIB=/usr/local/lib
SPELLIN=/usr/lib/spell/spellin
PUBLIC=/usr/local/lib
additions=/tmp/elst.$$
badlist=/tmp/ebad.$$
debug=x0
tmplist=/tmp/etmp.$$
locallist=$HOME/.spellwd
hashtable=$HOME/.spellht
badtable=/usr/lib/spell/hstop
history=/usr/lib/spell/spellhist
verbose=/dev/null
interactive=TRUE
files= flags=
: "*
   *	clean out /tmp before ending
   *"
trap "rm -f $tmplist $badlist $additions
	trap 0
	exit" 0 1 2 13 15
: "*
   *	parse for any options
   *"
for A in $*
do
	case $A in	
	-g)	locallist=/usr/local/lib
		hashtable=/usr/local/lib
		;;
	-s)	history=/dev/null
		;;
	-x*)	debug=$A
		;;
	-n)	interactive=FALSE
		;;
	*)	files="$files $A"
	esac
done
: "*
   *	create a local hashtable if one does not exist
   *" 
if test \! -f $hashtable
then
	echo 'making new hashtable'
	touch $locallist
	$SPELLIN /usr/lib/spell/hlista >$hashtable <$locallist
	cp /usr/lib/spell/hlista $hashtable
fi
: "*
   *	This series of pipes produces a list of
   *	possibly misspelled words
   *"
echo "running files through spell...\c"
deroff -w $files |\
  sort -u |\
  /usr/lib/spell/spellprog $badtable $badlist |\
  /usr/lib/spell/spellprog $hashtable $verbose $flags |\
  sort -u +0f +0 - $badlist |\
  tee -a $history >$badlist
who am i >>$history 2>/dev/null
echo done
: "*
   *	Now, if there are any misspelled words, send them through
   *	the interactive program to, possibly, correct the words in the file
   *	and produce a list of additions to the local dictionary.
   *"
if test -s $badlist -a $interactive = TRUE
then
	echo "Espel - interactive spelling checker	(type ? for help)"
	$LIB/espel.hi $badlist $additions $tmplist $debug $files
else 
	if test $interactive = FALSE
	then
		cat $badlist
	fi
fi
: "*
   *	Finally make any neccessary additions to the local dictionary
   *"
if test -s $additions
then
	echo 'updating local list (.spellwd)'
	cp $locallist $tmplist
	cat $tmplist $additions | sort -u >$locallist

	echo 'updating hashtable (.spellht)'
	cp $hashtable $tmplist
	$SPELLIN $tmplist >$hashtable <$additions
fi
