	.text
	.align  1
	.globl  _Getch
_Getch:
	movl    _point,r0
L190:
	cmpb    (r0)+,$32
	beql    L190
	movl    r0,_point
	movzbl  -(r0),r0
	rsb

	.globl  _Check
_Check:
	movl    _point,r1
L197:
	cmpb    (r1)+,$32
	beql    L197
	movzbl  -(r1),r0
	beql    L198
	cmpb    r0,$58
	beql    L198
	cmpl    r0,$233                 # ELSE = 0351
	bneq    L199
	tstb    _elsecount
	beql    L199
L198:
	movl    r1,_point
	rsb
L199:
	pushl   $1
	calls   $1,_error
	# no return since _error does not return

	.globl  __cleanup
	.globl	__exit
__cleanup:
	pushl   $0
	calls   $1,__exit
	ret
