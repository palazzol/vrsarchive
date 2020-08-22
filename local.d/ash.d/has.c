/*
**  Utility for ASH.
**	has list element
*/

exit(X) { _exit(X); }

main(ac, av)
    int			 ac;
    char		*av[];
{
    register char	*p;
    register char	*q;
    register int	 l;

    if (ac == 3)
	for (p = av[2], q = av[1], l = strlen(p); *q; q++)
	    if (*p == *q && strncmp(p, q, l) == 0)
		_exit(0);

    _exit(1);
}
