: /bin/sh
CBWLIB=`pwd`
SHELL=/bin/sh
LETTERSTATS=$CBWLIB/mss.stats
BIGRAMSTATS=$CBWLIB/mss-bigram.stats
TRIGRAMSTATS=$CBWLIB/trigrams.stats
DICTIONARY=$CBWLIB//word-list
export SHELL LETTERSTATS BIGRAMSTATS TRIGRAMSTATS DICTIONARY
#
#	This assumes the user's termcap has ks and ke, or the user can live
#	with vt100 style PF keys.  For systems without tset, you must have
#	have ks and ke.
#
if test -x /bin/tset || test -x /usr/bin/tset; then
  eval `tset -I -S | sed "s/';$/:ks=:ke=:k1=^[OP:k2=^[OQ:k3=^[OR:&/"`
fi
exec $CBWLIB/cbw $*
