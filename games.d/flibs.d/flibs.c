#define	MAXFLIBS	10	/* number of flibs */
#define	MAXSTATES	4	/* states per flib */
#define	MAXPATTERN	51	/* maximum length of a pattern */

/*
 * generation number
 */
static long generation = 0;
/*
 * flibs[ flib_number ][ state ][ input_value ][ 0 ] = next_state
 *					       [ 1 ] = output_value
 */
#define	NEXT_STATE	0
#define	OUTPUT_VALUE	1
static char flibs[MAXFLIBS][MAXSTATES][2][2];
/*
 * pattern that flib tries to match
 */
static char pattern[MAXPATTERN];
static int size_pattern;
/*
 * the threshold of number of matches a flib must make
 * and the current highest and lowest scores
 */
static int best_threshold;
static int scores[MAXFLIBS];
static int sorted_indexes[MAXFLIBS];
static int rankings[MAXFLIBS];
#define	best_predictor	scores[0]
#define	worst_predictor	scores[MAXFLIBS-1]
/*
 * the threshold of when to mate (in a range from 0..99)
 */
static int mate_threshold;
/*
 * the number of the flib that was last mutated and the
 * one was the last offspring
 */
static int last_mutated;
static int last_offspring;
/*
 * the storage for the graph
 */
static int graph_x;
/*
 * the display and run modes
 */
static int disp_mode;
static int last_disp_mode;
static int run_mode;
#define	D_HELP	0
#define	D_STATS	1
#define	D_GRAPH	2
#define	D_FLIBS	3
#define	D_1FLIB	4
#define	R_STOP	0
#define	R_RUN	1
#define	R_STEP	2

static init_graph(), init_pattern(), init_screen(), init_thresholds();

/*
 * And here is the main body of the program.
 */
main(argc,argv)
int argc;
char *argv[];
{
  /*
   * Initialize all the subsystems
   */
  Tinit();		/* initialize terminal control package */
  Oinit();		/* initialize operating system interface package */
  /*
   * Initialize variables
   */
  init_pattern(0);	/* generate a random pattern */
  init_thresholds();	/* generate the threshold */
  init_graph();		/* initialize the graph */
  init_flibs();		/* generate the flibs */
  init_screen();	/* draw the initial screen */
  /*
   * Continue looping the program until the user gives
   * up or the threshold condition is met.
   */
  while(1)
  {
	if( last_disp_mode == -1 ) goto start;
	check_winner();
	if( process_user_input() ) goto done;
	if( run_mode != R_STOP )
	{	generation++;
		display_screen1();	/* display generation number */
		flib_the_flibs();	/* process flibs and order them */
		crossover_mating();	/* do mating */
		randomly_mutate();	/* mutate one of the flibs */
		display_screen2();	/* display best,worst,etc. */
	}
	start:
	if( disp_mode != last_disp_mode )
		erase_display_area();
	switch( disp_mode )	/* display the currently selected details */
	{ case D_GRAPH:
		display_graph(); break;
	  case D_HELP:
		display_help(); break;
	  case D_STATS:
		display_stats(); break;
	  case D_FLIBS:
		display_flibs(); break;
	  case D_1FLIB:
		display_one_flib(); break;
	}
	last_disp_mode = disp_mode;
	Trefresh();
  }
  done:
  /*
   * Terminate all the subsystems
   */
  Tterminate();
  Oterminate();
}

/*
 * Initialize a random pattern
 *
 * Generate a random length for the repeating pattern.
 * Fill that much of the pattern up.  Then copy that
 * pattern over the remaining part of the pattern array.
 */
static init_pattern(length)
int length;
{ int i,j;
  if( !length )
  {	length = Orandom(4,8);
	for(i=0; i<length; i++)
		pattern[i] = Orandom(0,1);
  }
  for(i=0,j=length; j<MAXPATTERN; i++,j++)
  {	if( i >= length ) i -= length;
	pattern[j] = pattern[i];
  }
  size_pattern = length;
}

/*
 * Generate the two threshold numbers
 */
static init_thresholds()
{
  best_threshold = (int)( (float)(MAXPATTERN-1) * 0.9 );
  mate_threshold = Orandom(50,70);
}

/*
 * Initialize all the parameters having to do with
 * the graph.
 */
static init_graph()
{
  graph_x = 0;
}

/*
 * Randomly create all the flibs
 */
static int generate_flibs()
{ int i,j;
  for(i=0; i<MAXFLIBS; i++)
  {
	for(j=0; j<MAXSTATES; j++)
	{
		flibs[i][j][0][NEXT_STATE]   = Orandom(0,MAXSTATES-1);
		flibs[i][j][0][OUTPUT_VALUE] = Orandom(0,1);
		flibs[i][j][1][NEXT_STATE]   = Orandom(0,MAXSTATES-1);
		flibs[i][j][1][OUTPUT_VALUE] = Orandom(0,1);
	}
  }
}

/*
 * Display the initial screen and then after <RETURN>
 * display the real screen.
 */
static init_screen()
{ char *rev = "$Revision 1.0 $";
  static char *intro[] = {
"This is an implementation of A.K.Dewdney's FLIBs program from the November\n",
"1985 issue of Scientific American (Computer Recreations).\n",
"\n",
"\n",
"FLIBS are little four state state machines floating in a soup of repetitive\n"
,"binary sequences.  The FLIBs mutate, mate, live and die in an attempt to\n",
"be the best predictor of the sequence.\n",
"\n",
"This is evolution at its best...\n",
0,	};
  int i;
  Tcls(); Tstandout();
  Tcenter("    Dewdney's Flibs    ",0); Tstandend();
  Tcenter("implemented by Bjorn Benson",1);
  rev[13] = '\0'; Tcenter(rev+1,2);

  Tmoveto(0,4);
  for(i=0; intro[i]; i++)
	Tprintf(intro[i]);
  Tmoveto(0,23);
  Tprintf("Enjoy  (oh, and press return to start)");
  Trefresh();
  Tgetonekey();
  Tcls(); Tstandout();
  Tcenter("    Dewdney's Flibs    ",0); Tstandend();
  Tmoveto(0,1); Tprintf("Generation: ");
  Tmoveto(39,1); Tprintf("Best: ");
  Tmoveto(0,2); Tprintf("Pattern: ");
  for(i=0; i<MAXPATTERN; i++)
	Tpchar( pattern[i]+'0' );
  Trefresh();
  disp_mode = D_HELP;
  last_disp_mode = -1;
  run_mode = R_STOP;
}

/*
 * Randomly create all the flibs
 */
init_flibs()
{ int i,j,k;
  for(i=0; i<MAXFLIBS; i++)
  {
	for(j=0; j<MAXSTATES; j++)
	{
		for(k=0; k<2; k++)
		{
			flibs[i][j][k][NEXT_STATE]   = Orandom(0,3);
			flibs[i][j][k][OUTPUT_VALUE] = Orandom(0,1);
		}
	}
  }
}

/*
 * Display the first part of the screen
 */
display_screen1()
{
  Tmoveto(12,1); Tprintf("%ld", generation);
  Trefresh();
}

/*
 * Flibing the flibs means executing them as a state
 * machine and ranking their prediction values.
 */
flib_the_flibs()
{ char flibpat;
  int flibstate;
  int i,j,k,cnt;

  for(i=0; i<MAXFLIBS; i++)
  {
	flibstate = 0;
	for(cnt=k=0; k<MAXPATTERN; k++)
	{
		flibpat   = flibs[i][flibstate][pattern[k]][OUTPUT_VALUE];
		flibstate = flibs[i][flibstate][pattern[k]][NEXT_STATE];
		if( k+1<MAXPATTERN && flibpat == pattern[k+1] ) cnt++;
	}
	for(k=0; k<i; k++)
		if( cnt > scores[k] ) break;
	for(j=i; j>k; j--)
	{	scores[j] = scores[j-1];
		sorted_indexes[j] = sorted_indexes[j-1];
	}
	scores[k] = cnt;
	sorted_indexes[k] = i;
  }
  for(i=0; i<MAXFLIBS; i++)
	rankings[sorted_indexes[i]] = i;
}

/*
 * This checks to see if a crossover mating should occur, 
 * and if so, it does so.
 */
crossover_mating()
{ int c1,c2,i;
  char *p,*q;
  last_offspring = -1;
  if( Orandom(1,100) > mate_threshold ) return;
  last_offspring = sorted_indexes[MAXFLIBS-1];
  c1 = Orandom(0,(MAXSTATES*4)-1);
  c2 = Orandom(0,(MAXSTATES*4)-1);
  if( c1 > c2 )
  {	i = c1; c1 = c2; c2 = i;	}
  p = &(flibs[ sorted_indexes[MAXFLIBS-1]][ c1 / 4 ][0][0]) + (c1 % 4);
  q = &(flibs[ sorted_indexes[0]         ][ c1 / 4 ][0][0]) + (c1 % 4);
  c2 -= c1;
  while( c2-- )
	*p++ = *q++;
}

/*
 * An gamma-ray from outer space (or perhaps from Three Mile Island)
 * strikes a random flib and mutates it.
 */
randomly_mutate()
{ int flib,state,input,locus;
  flib  = Orandom(0,MAXFLIBS-1);
  state = Orandom(0,MAXSTATES-1);
  input = Orandom(0,1);
  locus = Orandom(0,1);
  if( locus == NEXT_STATE )
	flibs[flib][state][input][locus] = Orandom(0,MAXSTATES-1);
  else	flibs[flib][state][input][locus] = Orandom(0,1);
  last_mutated = flib;
}

/*
 * Display the second part of the screen
 */
display_screen2()
{
  Tmoveto(45,1); Tprintf("%2d  %5.2f%%", best_predictor,
	(((float)best_predictor) / ((float)(MAXPATTERN-1)))*100.0 );
}

/*
 * Display the best and worst predictors as a little graph.
 */
display_graph()
{ int y,wy;
  static long old_gen = -1;
  if( generation == old_gen ) return;
  old_gen = generation;
  for(y=23; y>3; y--)
  {	Tmoveto(graph_x,y); Tpchar(' ');	}
  wy = (int)(((float)worst_predictor / (float)(MAXPATTERN-1)) * 40.0);
  Tmoveto(graph_x,23-(wy>>1));
  Tpchar( (wy & 1) ? '-' : '_' );
  y = (int)(((float)best_predictor / (float)(MAXPATTERN-1)) * 40.0);
  Tmoveto(graph_x,23-(y>>1));
#ifdef STANDOUT
  Tstandout();
#endif
  Tpchar( (y & 1) ? '-' : '_' );
#ifdef STANDOUT
  for(wy=23-(wy>>1),y=23-(y>>1)+1; y<wy; y++)
  {	Tmoveto(graph_x,y);
	Tpchar(' ');
  }
  Tstandend();
#endif
  if( ++graph_x == 80 ) graph_x = 0;
}

/*
 * Display all the flibs
 */
display_flibs()
{ int i,j;
  for(i=0; i<MAXFLIBS; i++)
  {
	Tmoveto(0,4+i);
	Tpchar( (i == last_mutated) ? '*' : ' ');
	Tpchar( (i == last_offspring) ? '>' : ' ');
	Tpchar( ' ' );
	for(j=0; j<MAXSTATES; j++)
	{
		Tpchar( flibs[i][j][0][OUTPUT_VALUE]+'0' );
		Tpchar( flibs[i][j][0][NEXT_STATE]+'A'   );
		Tpchar( flibs[i][j][1][OUTPUT_VALUE]+'0' );
		Tpchar( flibs[i][j][1][NEXT_STATE]+'A'   );
	}
	Tprintf("   ");
	Tprintf("%2d", rankings[i]+1);
	Tprintf(" %2d", scores[rankings[i]]);
  }
}

/*
 * Display the help message
 */
display_help()
{
  if( disp_mode == last_disp_mode ) return;
  Tstandout();
  Tcenter("Commands Available",4);
  Tstandend();
  Tmoveto(0,5);  Tprintf("___Display Mode___");
  Tmoveto(0,6);  Tprintf("f   all flibs shown");
  Tmoveto(0,7);  Tprintf("g   graph of highest and lowest flib");
  Tmoveto(0,8);  Tprintf("h   help -- this screen");
  Tmoveto(0,9);  Tprintf("t   thresholds and other stats");
  Tmoveto(0,10); Tprintf("1   the single top flib");

  Tmoveto(39,5); Tprintf("___Changing Parameters___");
  Tmoveto(39,6); Tprintf("m   change mating threshold");
  Tmoveto(39,7); Tprintf("p   change the pattern");
  Tmoveto(39,8); Tprintf("w   change win threshold");

  Tmoveto(0,13); Tprintf("___Run Mode___");
  Tmoveto(0,14); Tprintf("r    run");
  Tmoveto(0,15); Tprintf("s    stop");
  Tmoveto(0,16); Tprintf("<CR> single step");
  Tmoveto(0,17); Tprintf("q    quit");
}

/*
 * Display various statistics and internal numbers
 */
display_stats()
{ int i;
  if( disp_mode == last_disp_mode ) return;
  Tmoveto(0,5); Tprintf("Mating threshold (0-100): %2d", mate_threshold);
  Tmoveto(0,6); Tprintf("Winner threshold (0-100): %5.2f",
	((float)best_threshold / (float)(MAXPATTERN-1))*100.0);
  Tmoveto(0,7); Tprintf("Pattern length: %d", size_pattern);
  Tmoveto(0,8); Tprintf("Subset pattern: ");
  for(i=0; i<size_pattern; i++)
	Tpchar( pattern[i]+'0' );
}

/*
 * Display just the top flib
 */
display_one_flib()
{ int i,j,k,y;
  Tstandout();
  Tmoveto(8,6);  Tprintf("      0     1   ");
  Tmoveto(8,7);  Tprintf("   ");
  Tmoveto(8,8);  Tprintf(" A ");
  Tmoveto(8,9);  Tprintf("   ");
  Tmoveto(8,10); Tprintf(" B ");
  Tmoveto(8,11); Tprintf("   ");
  Tmoveto(8,12); Tprintf(" C ");
  Tmoveto(8,13); Tprintf("   ");
  Tmoveto(8,14); Tprintf(" D ");
  Tmoveto(8,15); Tprintf("   ");
  Tstandend();
  i = sorted_indexes[0];
  for(j=0,y=8; j<MAXSTATES; j++,y+=2)
  {	Tmoveto(13,y);
	Tpchar( flibs[i][j][0][NEXT_STATE]+'A' );   Tpchar( ' ' );
	Tpchar( flibs[i][j][0][OUTPUT_VALUE]+'0' );
	Tmoveto(19,y);
	Tpchar( flibs[i][j][1][NEXT_STATE]+'A' );   Tpchar( ' ' );
	Tpchar( flibs[i][j][1][OUTPUT_VALUE]+'0' );
  }
}

/*
 * Erase the displaying region
 */
erase_display_area()
{
  Tmoveto(0,4);
  Tcleos();
}

/*
 * Check to see if any flib has attained
 * winner status.
 */
check_winner()
{
  if( best_predictor >= best_threshold )
	run_mode = R_STOP;
}

/*
 * Get and process any user input
 */
int process_user_input()
{ int c,i;
  if( run_mode == R_RUN && !Tkey_avail() ) return( 0 );
  c = Tgetonekey();
  if( c >= 'A' && c <= 'Z' ) c += 'a' - 'A';
  switch( c )
  { case 'f':
	disp_mode = D_FLIBS;
	break;
    case 'g':
	disp_mode = D_GRAPH;
	break;
    case 'h':
	disp_mode = D_HELP;
	break;
    case 't':
	disp_mode = D_STATS;
	break;
    case '1':
	disp_mode = D_1FLIB;
	break;
    case 'm':
	Tmoveto(39,3);
	Tprintf("Mate threshold: [  ]\b\b\b");
	gettwodigit( &mate_threshold );
	if( disp_mode == D_STATS ) last_disp_mode = -1;
	Tmoveto(39,3);
	Tprintf("                    ");
	break;
    case 'p':
	Tmoveto(9,2);
	Tprintf("                                        ");
	Tprintf("                              ");
	Tmoveto(8,2);
	Tpchar('[');
	Trefresh();
	i = 0;
	while(1)
	{	c = Tgetonekey();
		if( c != '0' && c != '1' )
		{	if( c == '\r' || c == '\n' ) goto done;
			putchar('\007');
			continue;
		}
		pattern[i++] = c-'0';
		Tpchar(c);
		Trefresh();
	}
	done:
	Tmoveto(8,2); Tpchar(' ');
	init_pattern(i);
	Tmoveto(9,2);
	for(i=0; i<MAXPATTERN; i++)
		Tpchar( pattern[i]+'0' );
	Trefresh();
	if( disp_mode == D_STATS ) last_disp_mode = -1;
	break;
    case 'w':
	Tmoveto(39,3);
	Tprintf("Winner threshold %: [  ]\b\b\b");
	gettwodigit( &best_threshold );
	best_threshold = (int)((float)best_threshold * (float)(MAXPATTERN-1))
					/100.0;
	if( disp_mode == D_STATS ) last_disp_mode = -1;
	Tmoveto(39,3);
	Tprintf("                      ");
	break;
    case 'r':
	run_mode = R_RUN;
	break;
    case 's':
	run_mode = R_STOP;
	break;
    case 'q':
	return( 1 );
    case '\r': case '\n':
	run_mode = R_STEP;
	break;
    default:
	putchar('\007');
	break;
  }
  return( 0 );
}

/*
 * Get a two digit number
 */
gettwodigit( val )
int *val;
{ int c1,c2;
  Trefresh();
  loop1:
  c1 = Tgetonekey();
  if( c1 < '0' || c1 > '9' )
  {	putchar('\007');
	goto loop1;
  }	
  Tpchar(c1);
  Trefresh();
  loop2:
  c2 = Tgetonekey();
  if( c2 < '0' || c2 > '9' )
  {	putchar('\007');
	goto loop2;
  }	
  Tpchar(c2);
  Trefresh();
  *val = (c1-'0') * 10 + (c2-'0');
}
