
#include <curses.h>

main(argc, argv)
int argc;
char *argv[];
{
	char c1, c2, *s;
	int x, y;

	if (--argc <= 0) {
		fprintf(stderr,"Usage: curse -xx ...\n");
		exit(0);
	}
	initscr();
	while (--argc >= 0) {
	    *++argv;
	    if ((*argv)[0] == '-') {
		c1 = (*argv)[1];
		if ((c1 >= 'A') && (c1 <= 'Z')) c1 = c1+'a'-'A';
		c2 = (*argv)[2];
		if ((c2 >= 'A') && (c2 <= 'Z')) c2 = c2+'a'-'A';

		if ((c1 == 'x') && (c2 == 'x')) {
			printf("Legal options are:\n");
			printf("al,bc,bt,cd,ce,cl,cm,dc,dl,dm,do,ed,ei,ho\n");
			printf("ic,im,ip,ll,ma,nd,pc,se,sf,so,sr,ta,te,ti\n");
			printf("uc,ue,ul,us,vb,ve,vs\n");
			printf("Numeric valued capabilities:\n");
			printf("li and co\n");
		}
		else if ((c1 == 'a') && (c2 == 'l')) printf("%s",AL);
		else if ((c1 == 'b') && (c2 == 'c')) printf("%s",BC);
		else if ((c1 == 'b') && (c2 == 't')) printf("%s",BT);
		else if ((c1 == 'c') && (c2 == 'd')) printf("%s",CD);
		else if ((c1 == 'c') && (c2 == 'e')) printf("%s",CE);
		else if ((c1 == 'c') && (c2 == 'l')) printf("%s",CL);
		else if ((c1 == 'c') && (c2 == 'm')) {
			if ((argc -= 2) < 0) {
				fprintf(stderr,"Usage: curse -cm X Y\n");
				exit(0);
			}
			x = (char)atoi(*++argv);
			y = (char)atoi(*++argv);
			move(x,y);
		}
		else if ((c1 == 'c') && (c2 == 'o')) printf("%d",COLS);
		else if ((c1 == 'd') && (c2 == 'c')) printf("%s",DC);
		else if ((c1 == 'd') && (c2 == 'l')) printf("%s",DL);
		else if ((c1 == 'd') && (c2 == 'm')) printf("%s",DM);
		else if ((c1 == 'd') && (c2 == 'o')) printf("%s",DO);
		else if ((c1 == 'e') && (c2 == 'd')) printf("%s",ED);
		else if ((c1 == 'e') && (c2 == 'i')) printf("%s",EI);
		else if ((c1 == 'h') && (c2 == 'o')) {
			if (*HO != '\0') printf("%s",HO);
			else move(0,0);
		}
		else if ((c1 == 'i') && (c2 == 'c')) printf("%s",IC);
		else if ((c1 == 'i') && (c2 == 'm')) printf("%s",IM);
		else if ((c1 == 'i') && (c2 == 'p')) printf("%s",IP);
		else if ((c1 == 'l') && (c2 == 'i')) printf("%d",LINES);
		else if ((c1 == 'l') && (c2 == 'l')) {
			if (*LL != '\0') printf("%s",LL);
			else move(0,LINES-1);
		}
		else if ((c1 == 'm') && (c2 == 'a')) printf("%s",MA);
		else if ((c1 == 'n') && (c2 == 'd')) printf("%s",ND);
		else if ((c1 == 'p') && (c2 == 'c')) printf("%s",PC);
		else if ((c1 == 's') && (c2 == 'e')) printf("%s",SE);
		else if ((c1 == 's') && (c2 == 'f')) printf("%s",SF);
		else if ((c1 == 's') && (c2 == 'o')) printf("%s",SO);
		else if ((c1 == 's') && (c2 == 'r')) printf("%s",SR);
		else if ((c1 == 't') && (c2 == 'a')) printf("%s",TA);
		else if ((c1 == 't') && (c2 == 'e')) printf("%s",TE);
		else if ((c1 == 't') && (c2 == 'i')) printf("%s",TI);
		else if ((c1 == 'u') && (c2 == 'c')) printf("%s",UC);
		else if ((c1 == 'u') && (c2 == 'e')) printf("%s",UE);
		else if ((c1 == 'u') && (c2 == 'l')) printf("%s",UL);
		else if ((c1 == 'u') && (c2 == 's')) printf("%s",US);
		else if ((c1 == 'v') && (c2 == 'b')) printf("%s",VB);
		else if ((c1 == 'v') && (c2 == 'e')) printf("%s",VE);
		else if ((c1 == 'v') && (c2 == 's')) printf("%s",VS);
		else fprintf(stderr,"curse: unknown option %s\n",*argv);
	    }
	    else for (s = argv[0]; *s != '\0'; s++) putchar(*s);
	}
	endwin();
}

move(x,y)
int x, y;
{
	char n, *s;
	int i = 0, rev = 0;

	for (s = CM; *s != '\0'; s++) {
		if (*s != '%') putchar(*s);
		else {
			switch (i) {
				case 0:	n = (rev?x:y);
					break;
				case 1:	n = (rev?y:x);
					break;
				default: n = '\0';
			}
			switch(*++s) {
				case 'd':	printf("%d",n);
						i++;
						break;
				case '2':	printf("%2d",n);
						i++;
						break;
				case '3':	printf("%3d",n);
						i++;
						break;
				case '+':	n += (int)*++s;
				case '.':	printf("%c",(char)n);
						i++;
						break;
				case '>':	if (n > (int)*++s) n+=(int)*++s;
						else *++s;
						break;
				case 'i':	x++; y++;
						break;
				case 'n':	x ^= 0140; y ^= 0140;
						break;
				case 'B':	x = (16*(x/10))+(x%10);
						y = (16*(y/10))+(y%10);
						break;
				case 'D':	x = (x-2*(x%16));
						y = (y-2*(y%16));
				case 'r':	rev++;
						break;
				default:	*--s;
				case '%':	break;
			}
		}
	}
}
