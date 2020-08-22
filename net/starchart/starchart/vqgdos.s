	.globl	vqgdos_
	.shri
vqgdos_:move	$-2, d0
	trap	$2
	cmpi	$-2, d0
	sne	d0
	ext	d0
	rts
