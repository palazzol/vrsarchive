/*  Version 1.0		6/12/85
	Jim Hein
	Nicolet Instrument Corp
	5225 Verona Rd
	Madison, WI  53611-0288
*/
main(argc,argv)
int argc;
char *argv[]; {
	char buf[512];
	register int i;

	for (i = 1; i < argc; i++) {
		DELCR(argv[i]);
	}
}
