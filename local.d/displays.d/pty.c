/*
 *	Pty manipulation routines.
*/
#include <errno.h>
#include <signal.h>
#include "globals.h"
#include "pipe.h"

extern int errno;
extern void done();

int pty_fd[MAXVDISP];
int pty_pid[MAXVDISP];

char *ptynames[] = {
	"/dev/ptyv0",
	"/dev/ptyv1",
	"/dev/ptyv2",
	"/dev/ptyv3",
	"/dev/ptyv4",
	"/dev/ptyv5",
	"/dev/ptyv6",
	"/dev/ptyv7",
	0,
};

/*
 *	Open a pty, then fork a child process to copy output into the pipe.
*/
open_pty(pipe_fd, id)
int pipe_fd;		/* Pipe to communicate on	*/
int id;			/* id to send with characters	*/
{
	static int nextpty = 0;
	int fd = -1, i;
	struct pipe_msg buf;

	while (fd == -1) {
		if (ptynames[nextpty] == 0) {
			perror("pty open");
			exit(1);
		}
		fd = open(ptynames[nextpty++], 2);
	}
	pty_pid[id] = fork();
	if (pty_pid[id] == 0) {
		for (i = 0; i < id; i++)
			close(pty_fd[i]);
		buf.id = id;
		while (1) {
			buf.count = read(fd, &buf.ch, 1);
			(void) write(pipe_fd, (char *)&buf, sizeof buf);
		}
	} else
		pty_fd[id] = fd;
}

/*
 *	Initialize all required ptys and their associated virtual displays.
*/
pty_init(pipe_fd)
int pipe_fd;		/* pipe fd */
{	int i;

	for (i = 0; i < MAXVDISP; i++)
		open_pty(pipe_fd, i);
	vdisp_init();
}

/*
 *	Write a character to a pty.
*/
put_pty(vdisp, ch)
char ch;
{
	int fd;

	fd = pty_fd[vdisp];
	return(write(fd, &ch, 1));
}

/*
 * Clean up and exit to the shell.
 */
void
done(status)
int status;
{	int i;

	keybd_wrapup();
	vdisp_wrapup();
	pdisp_wrapup();
	for (i = 0; i < MAXVDISP; i++) {
		kill(pty_pid[i], SIGTERM);
		wait((int *)0);
	}
	exit(status);
}
