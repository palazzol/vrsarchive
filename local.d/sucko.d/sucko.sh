#!/bin/sh
#
#	Verify operating parameters
#
if test ! -f .suck; then
    echo $* >.suck
fi
if test ! -f .sucked; then
    touch .sucked
fi
#
#	Suck up the news
#
cp .sucked .sucked-
sucklst .sucked `cat .suck` | sort -o .sucked+
if test -s .sucked+; then
    mv .sucked+ .sucked
fi
#
#	Remove duplication, Process local filter script
#
folder +`pwd`
sortm
if test -x .kill; then
    ./.kill
fi
fgrep "Message-ID:" [1-9]* /dev/null |
sort -t: +2 |
awk -F: '
	BEGIN   	{ prev = ""; art = 0; }
	prev == $3	{ print art, $1; next; }
				{ art=$1; prev = $3; next; }
' |
while read a b; do
	if cmp $a $b >/dev/null; then
		rm $b
	else
		echo $a $b!
		: exit 1
	fi
done
folder -pack
scan | tail -24
