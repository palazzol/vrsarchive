: /bin/sh
#
#   Sort the current directory
#
MAGIC='"dirsort.wrk"'	# Double quotes reduce collisions
case $# in
  0) ;;
  1) cd $1 ;;
  *) echo "Usage: dirsort [dir]" >&2
     exit 1
esac
#
while :; do
  CURRENT=`ls -f .`
  for i in .* *; do
    #
    #	Obtain the name of the file currently occupying the desired slot.
    #
    set $CURRENT
    INSLOT=$1
    case $# in
      1) ;;
      *) shift
    esac
    CURRENT="$*"
    #
    #	Now deal with the cases
    #
    case $i in
      .|..|$INSLOT) ;;
      *) #
  	 #	Deal with the case where the wrong file is in the slot.
  	 #
  	 mv $i ${MAGIC}1
  	 mv $INSLOT ${MAGIC}2
  	 mv ${MAGIC}1 $i
  	 mv ${MAGIC}2 $INSLOT
	 continue 2	# Restart the scan
    esac
  done
  break		# Completed scan without swaps
done
