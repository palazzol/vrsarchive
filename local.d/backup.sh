: /bin/sh
#
#	Back up to (and restore from) portable cpio format tape.
#
set -- `getopt iotf: $*`
case $? in
  0) ;;
  *) echo "Usage: backup <-i|-o|-t> [-f file] <directory>..." 1>&2
     exit 2
esac
DIRS=""
MODE="list"
TAPE="/dev/rmt0"
while :; do
  case $i in
    -c) MODE="dump" ;;
    -t) MODE="list" ;;
    -x) MODE="load" ;;
    -f) TAPE=$2
        shift
        ;;
    --)	shift ;;
    *) 	DIRS="$DIRS $1" ;;
  esac
  shift
done
#
#	Now do it
#
case $MODE in
  load) cpio -iBcvd <$TAPE ;;
  dump) find $DIRS -print | sort | cpio -oBcv >$TAPE ;;
  list) cpio -itBcv <$TAPE ;;
esac
