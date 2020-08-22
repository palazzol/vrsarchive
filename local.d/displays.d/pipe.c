/*
 *	Pipe manipulation routines.
*/
#include <errno.h>
#include <signal.h>
#include "globals.h"
#include "pipe.h"

extern int errno;
extern void done();

int read_pipe;

/*
 *	Initialize all required ptys and their associated virtual displays.
*/
pipe_init()
{	int flags;
	int pipes[2];

	if (pipe(pipes) == -1) {
		perror("pipe");
		exit(1);
	}
	pty_init(pipes[1]);
	keybd_init(pipes[1]);
	close(pipes[1]);
	read_pipe = pipes[0];
}

/*
 *	Process characters from the pipe, if any.
 *	ret == -1 => error, ret == 0 => No character available.
*/
pchar()
{	int ret;
	struct pipe_msg buf;

	while (1) {
		ret = read(read_pipe, (char *)&buf, sizeof(buf));
		if (ret == 0)
			return;
		if ((ret == -1) && (errno == EAGAIN))
			return;
		if (ret == -1) {
			perror("pchar read");
			done(1);
		}
		if (buf.count) {
			if (buf.id == -1)
				kchar(buf.ch);
			else
				wputc(buf.id, buf.ch);
		}
	}
}
