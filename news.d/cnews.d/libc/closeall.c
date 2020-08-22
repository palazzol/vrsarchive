#include <sys/param.h>

closeall(leavestd)
int leavestd;
{
	register int i;

	for (i = (leavestd? 3: 0); i < NOFILE; i++)
		close(i);
}
