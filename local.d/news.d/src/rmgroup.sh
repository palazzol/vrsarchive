: '@(#)rmgroup.sh	1.3 8/21/84'
for group
do
	echo "Removing newsgroup $group"
	qgrp="`echo $group | sed 's/\./\\\./g'`"
	if
		grep -s "^$qgrp " LIBDIR/active
	then
		ed - LIBDIR/active << E_O_F
/^$qgrp /d
w
q
E_O_F
		dir=SPOOLDIR/"`echo $group | sed 's/\./\//g'`"
		if
			[ -d "$dir" ]
		then
			rm -r "$dir"
		else
			echo "$0: $dir: no spool directory" 2>&1
		fi
	else
		echo "$0: $group: no such newsgroup" 2>&1
	fi
done
exit 0
