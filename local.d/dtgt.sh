: /bin/sh
#
#	Print the date for the record
#
date
#
#	Check usage and note arguments
#
case $# in
  2) ;;
  *) echo "usage: $0 <dir1> <dir2>" >&2
     exit 1
esac
DIR1=$1 export DIR1
DIR2=$2 export DIR2
#
#	Arrange cleanup on exit, exit on signal
#
trap "rm -f /tmp/lst1_$$ /tmp/lst2_$$" 0
trap "exit 1" 1 2 3
#
#	Make lists of files from each directory
#
(cd $DIR1; find . -type f -print) | sort -o /tmp/lst1_$$
(cd $DIR2; find . -type f -print) | sort -o /tmp/lst2_$$
#
#	Make lists of files only in one, the other, or both
#
comm -23 /tmp/lst1_$$ /tmp/lst2_$$ >dir1
comm -13 /tmp/lst1_$$ /tmp/lst2_$$ >dir2
comm -12 /tmp/lst1_$$ /tmp/lst2_$$ >both
#
#	For each file in both, are they equivalent?
#
while read f; do
  bincmp $DIR1/$f $DIR2/$f
done <both 2>&1 |\
sed "s:^$DIR1.::
     s/ .*//" >diff
#
#	Note the date again, for the record
#
date
