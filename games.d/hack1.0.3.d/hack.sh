: /bin/sh
HACKDIR=/usr/games/lib/hack1.0.3
HACK=$HACKDIR/hack
MAXNROFPLAYERS=4

cd $HACKDIR
case $1 in
	-s*)
		exec $HACK $@
		;;
	*)
		exec $HACK $@ $MAXNROFPLAYERS
		;;
esac
