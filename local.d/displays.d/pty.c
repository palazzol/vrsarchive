/*
 *	Pty manipulation routines.
*/
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "globals.h"

extern int errno;
extern void done();

int pty_pipe;
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

struct pipe_msg {
	int id;
	int count;
	char ch;
};

/*
 *	Open a pty, then fork a child process to copy output into the pipe.
*/
open_pty(pipes, id)
int pipes[2];		/* Pipes to communicate on	*/
int id;			/* id to send with characters	*/
{
	static int nextpty = 0;
	int fd = -1;
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
		close(pipes[0]);
		buf.id = id;
		while (1) {
			buf.count = read(fd, &buf.ch, 1);
			(void) write(pipes[1], (char *)&buf, sizeof buf);
		}
	} else
		pty_fd[id] = fd;
}

/*
 *	Initialize all required ptys and their associated virtual displays.
*/
pty_init()
{	int i, flags;
	int pipes[2];

	if (pipe(pipes) == -1) {
		perror("pipe");
		exit(1);
	}
	for (i = 0; i < MAXVDISP; i++)
		open_pty(pipes, i);
	close(pipes[1]);
	pty_pipe = pipes[0];
	flags = fcntl(pty_pipe, F_GETFL, 0);
	if (flags == -1) {
		perror("pty_pipe F_GETFL");
		exit(1);
	}
	if (fcntl(pty_pipe, F_SETFL, flags|O_NDELAY) == -1) {
		perror("pty_pipe F_SETFL");
		exit(1);
	}
	vdisp_init();
}

/*
 *	Process characters from the pipe, if any.
 *	ret == -1 => error, ret == 0 => No character available.
*/
pchar()
{	int ret;
	struct pipe_msg buf;

	while (1) {
		ret = read(pty_pipe, (char *)&buf, sizeof(buf));
		if (ret == 0)
			return;
		if ((ret == -1) && (errno == EAGAIN))
			return;
		if (ret == -1) {
			perror("pchar read");
			done(1);
		}
		if (buf.count)
			wputc(buf.id, buf.ch);
	}
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

	pwrapup();
	for (i = 0; i < MAXVDISP; i++) {
		kill(pty_pid[i], SIGTERM);
		wait((int *)0);
		sleep(2);
	}
	exit(status);
}
