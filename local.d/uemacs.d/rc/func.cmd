;	FUNC.CMD	Allow mainframes to simulate function
;			keys with ^C<n> and ^C shifted-<n>
;			Also allow the ` key to be META as well as ESC

bind-to-key execute-macro-21 ^C

21	store-macro
	!if &not $pending
		write-message "FN-"
	!endif
	set %rcchar &gtkey
	set %rcchar &sindex "1234567890!@#$%^&*()" %rcchar
	!if &equ %rcchar 0
		write-message "[Not Bound]"
		!return
	!endif
	clear-message-line
	set %rcchar &mid ";<=>?@ABCDTUVWXYZ[\]" %rcchar 1
	&ind &bind &cat "FN" %rcchar
!endm
