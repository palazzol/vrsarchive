: '
:	Log Phone calls from the field
: '
clear
: '
:	Init pointers to relevant files
: '
PHONEFILE=$HOME/phonefile
NAMES=/usr/local/lib/names
LOCK=/tmp/pl$$		# Set to unique name since each user now has a phonefile
: '
:	Obtain the data from the user.  Since this is slow, it is
:	done before the lock is set.
: '
echo "
Type <CR> to terminate input

Caller(s):"
NAME=`awk '{ if (NF == 0) exit(0); else print $0; }' /dev/tty`
echo  "Type in the question (be brief)"
QUESTION=`awk '{ if (NF == 0) exit(0); else print $0; }' /dev/tty`
echo "and your reply? "
REPLY=`awk '{ if (NF == 0) exit(0); else print $0; }' /dev/tty`
echo -n "SSS related question? "
read OK
echo -n "SSS Log Number? "
read NUMBER
echo -n "Time spent? "
read TIME
: '
:	Disallow interrupts and acquire lock.  Interrupts are
:	masked while the lock may be set.
: '
echo >/tmp/$$
trap '' 2 3 15
while true; do
  if ln /tmp/$$ $LOCK 2>/dev/null; then
    break			;: Have lock set
  else
    echo "Data file in use. Please stand by."
    sleep 2
  fi
done
rm /tmp/$$			;: Done with this
: '
:	Open PHONEFILE, dump collected data.  This is done with
:	the lock set and interrupts disabled, so no time consuming
:	steps from here on.
: '
exec >> $PHONEFILE
echo "============================"
date
echo "============================
NAME
----
$NAME"
grep -y "$NAME" $NAMES 2>/dev/null
echo "
QUESTION
--------
$QUESTION"
echo "
REPLY
-----
$REPLY"
echo "
SSS? $OK"
echo "LOG NUMBER: $NUMBER"
echo "
Time: $TIME "
rm $LOCK
