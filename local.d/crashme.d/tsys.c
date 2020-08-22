/*
 *	tsys - test syscall by calling syscall with random system call and
 *		varying number of args (some of which are known to be illegal).
 *
 *  9/18/91 Tin Le
 *
 * Email Address: tin@smsc.Sony.Com or try tin@szebra.Saigon.COM
 *
 * Copyright (c) 1991 By Tin Le, San Jose, California
 *	and Sony Microsystems, San Jose, California
 * 	Portions Copyrighted by GEORGE J. CARRETTE, CONCORD, MASSACHUSETTS.
 *	All rights reserved
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice appear
 * in supporting documentation, and that the name of the author
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL HE OR SONY MICROSYSTEMS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: tsys.c,v 1.1 1991-10-07 16:11:43 vrs Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.2  91/09/20  15:47:06  tin
 * Added sequential test of syscalls with random args.
 * Several minor bug fixes and enhancements.
 * 
 * Revision 1.1  91/09/19  09:52:45  tin
 * Initial revision
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/syscall.h>

#define MAXARGS		9
#define NUMSYSS		150
#define TIMEOUT		120	/* 2 minutes */

#ifndef SYSVoffset		/* I think only Sony uses this - TL */
#define SYSVoffset	0
#endif

int	call;
int	args;
char	*arglist[MAXARGS];

jmp_buf again_buff;

void again_handler(sig, code, scp, addr)
int sig, code;
struct sigcontext *scp;
char *addr;
{
	char *ss;
	switch(sig)
	{
		case SIGILL: ss =   " illegal instruction"; break;
		case SIGTRAP: ss =   " trace trap"; break;
		case SIGFPE: ss =   " arithmetic exception"; break;
		case SIGBUS: ss =  " bus error"; break;
		case SIGSEGV: ss =  " segmentation violation"; break;
		case SIGIOT: ss = " IOT instruction"; break;
		case SIGEMT: ss = " EMT instruction"; break;
		case SIGALRM: ss = " alarm clock"; break;
		case SIGINT:  ss = " interrupt"; break;
		case SIGSYS:  ss = " bad syscall"; break;
		default: ss = "";
	}
	signal(sig, again_handler);
	fprintf(stderr, "Got signal %d: %s", sig, ss);
	longjmp(again_buff, 3);
}

set_up_signals()
{
	printf("set_up_signals\n");

	signal(SIGILL, again_handler);
	signal(SIGTRAP, again_handler);
	signal(SIGFPE, again_handler);
	signal(SIGBUS, again_handler);
	signal(SIGSEGV, again_handler);
	signal(SIGIOT, again_handler);
	signal(SIGEMT, again_handler);
	signal(SIGALRM, again_handler);
	signal(SIGINT, again_handler);
	signal(SIGSYS, again_handler);
}

main()
{
	int	loop;

	printf("\nStarting tsys\n");

	/* seed rand() */
	srand((unsigned int)time((char *)NULL));

	/* Init args array */
	for (loop=0; loop<MAXARGS; loop++)
		arglist[loop] = NULL;
	arglist[1] = (char *)0x7f012345;
	arglist[2] = (char *)0x8f012345;
	arglist[4] = (char *)0xff012345;
	arglist[5] = (char *)0x0f5;

	/* catch signals */
	set_up_signals();

	for (loop=0; loop<1024; loop++) {
		if (setjmp(again_buff) == 3) {
			fprintf(stderr, "\nIt failed!\n");
			fflush(stderr);
		}
		call = rand() % NUMSYSS + SYSVoffset + 1;
		args = rand() % MAXARGS;
		fprintf(stderr, "syscall(%d, args=%d)\n", call, args);
		fflush(stderr);

		/* In case the syscall went south, we want to timeout */
		alarm(TIMEOUT);

		switch(args) {
		case 0:
			syscall(call);
			break;
		case 1:
			syscall(call, arglist[0]);
			break;
		case 2:
			syscall(call, arglist[0], arglist[1]);
			break;
		case 3:
			syscall(call, arglist[0], arglist[1], arglist[2]);
			break;
		case 4:
			syscall(call, arglist[0], arglist[1], arglist[2],
				arglist[3]);
			break;
		case 5:
			syscall(call, arglist[0], arglist[1], arglist[2],
				arglist[3], arglist[4]);
			break;
		case 6:
			syscall(call, arglist[0], arglist[1], arglist[2],
				arglist[3], arglist[4], arglist[5]);
			break;
		case 7:
			syscall(call, arglist[0], arglist[1], arglist[2],
				arglist[3], arglist[4], arglist[5], arglist[6]);
			break;
		case 8:
			syscall(call, arglist[0], arglist[1], arglist[2],
				arglist[3], arglist[4], arglist[5], arglist[6],
				arglist[7]);
			break;
		default:
			fprintf(stderr, "Error: call=%d, args=%d\n",
				call, args);
			break;
		}
	}

	/* Now we go through all the syscalls in order */
	for (loop=0; loop<64; loop++) {
		for (call=1; call<=NUMSYSS; call++) {
			if (setjmp(again_buff) == 3) {
				fprintf(stderr, "\nIt failed!\n");
				fflush(stderr);
			}
			args = rand() % MAXARGS;
			fprintf(stderr, "syscall(%d, args=%d)\n", call, args);
			fflush(stderr);

			/* In case the syscall went south, we want to timeout */
			alarm(TIMEOUT);

			switch(args) {
			case 0:
				syscall(call + SYSVoffset);
				break;
			case 1:
				syscall(call + SYSVoffset, arglist[0]);
				break;
			case 2:
				syscall(call + SYSVoffset, arglist[0],
					arglist[1]);
				break;
			case 3:
				syscall(call + SYSVoffset, arglist[0],
					arglist[1], arglist[2]);
				break;
			case 4:
				syscall(call + SYSVoffset, arglist[0],
					arglist[1], arglist[2], arglist[3]);
				break;
			case 5:
				syscall(call + SYSVoffset, arglist[0],
					arglist[1], arglist[2], arglist[3],
					arglist[4]);
				break;
			case 6:
				syscall(call + SYSVoffset, arglist[0],
					arglist[1], arglist[2], arglist[3],
					arglist[4], arglist[5]);
				break;
			case 7:
				syscall(call + SYSVoffset, arglist[0],
					arglist[1], arglist[2], arglist[3],
					arglist[4], arglist[5], arglist[6]);
				break;
			case 8:
				syscall(call + SYSVoffset, arglist[0],
					arglist[1], arglist[2], arglist[3],
					arglist[4], arglist[5], arglist[6],
					arglist[7]);
				break;
			default:
				fprintf(stderr, "Error: call=%d, args=%d\n",
					call, args);
				break;
			}
		}
	}
}
