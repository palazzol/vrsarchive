: '@(#)sendbatch.sh	1.3 8/21/84'
for rmt in $*
do
	while test $? -eq 0 -a \( -s BATCHDIR/$rmt -o -s BATCHDIR/$rmt.work \)
	do
		LIBDIR/batch BATCHDIR/$rmt 50000 | \
			if test -s BATCHDIR/$rmt.cmd
			then
				BATCHDIR/$rmt.cmd
			else
				uux - UUXFLAGS $rmt!rnews
			fi
	done
done
