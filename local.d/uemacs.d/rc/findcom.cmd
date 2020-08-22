;	This hunts down mismatched comment problems in C

;	start from the current position in the file

	!force search-forward "/*"
	!if &seq $status FALSE
		!goto nend
	!endif

*nxtopen
	;record the position of the open comment
	update-screen
	set %oline $curline
	set %opos $curcol

	;find the first close comment
	!force search-forward "*/"
	!if &seq $status FALSE
		write-message "%%No close to this open comment"
		!return
	!endif

	;record the position of the close comment
	set %cline $curline
	set %cpos $curcol

	;go back to the open and find the next open
	set $curline %oline
	set $curcol %opos

	;and now find the next open
	!force search-forward "/*"
	!if &seq $status FALSE
		write-message "No errors"
		!return
	!endif
	set %nline $curline
	set %npos $curcol

	;compare the close to the next open
	!if &less %cline %nline
		!goto getnext
	!endif
	!if &and &equ %cline %nline &less %cpos %npos
		!goto getnext
	!endif

	;report a mismatch
	set $curline %oline
	set $curcol %opos
	write-message "%%This comment does not terminate properly"
	!return

*getnext
	set $curline %nline
	set $curcol %npos
	!goto nxtopen 
