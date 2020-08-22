/*
 *	%W% %G%
 *	The vfork() is essentially equivalent in functionality to fork()
 *	call, but eliminates some copying in brain-damaged virtual UNIX
 *	implementations (which cannot implement copy-on-write).  We emulate
 *	it here with fork(), which is the more powerful primitive (and no
 *	more expensive on virtual memory systems that implement it with
 *	copy-on-write).
 *
 *	It would be faster to #define vfork() fork() in some include file.
 *	Unfortunately there is no include file which is a likely candidate.
 *	Currently, fork() is expensive enough that an extra procedure call
 *	is trivial anyway.
*/

int
vfork()
{ return(fork());
}
