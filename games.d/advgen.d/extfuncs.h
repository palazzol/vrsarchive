void	add_error();		/* advgram.y */
void	addspecialword();	/* specials.c */
void	addsym();		/* lookup.c */
void	add_treasure();		/* specials.c */
void	addword();		/* vocab.c */
symbol	*anon_msg();		/* msgs.c */
void	checkrooms();		/* rooms.c */
void	checkspecials();	/* specials.c */
void	codeassign();		/* code.c */
void	codeaugment();		/* code.c */
void	codebyte();		/* code.c */
void	codeinc();		/* code.c */
void	codeprint();		/* code.c */
int	codereln();		/* code.c */
void	confdecl();		/* error.c */
void	decl_item();		/* items.c */
void	decl_msg();		/* msgs.c */
int	decl_room();		/* rooms.c */
void	decl_var();		/* vars.c */
void	dumpline();		/* advlex.l */
void	endcond();		/* code.c */
void	enddaemons();		/* code.c */
void	endrule();		/* code.c */
void	fatal();		/* error.c */
int	findnoun();		/* vocab.c */
int	findverb();		/* vocab.c */
int	firstroom();		/* rooms.c */
char	*getenv();		/* C library */
void	gramerror();		/* error.c */
void	greetmsg();		/* msgs.c */
void	init_nouns();		/* specials.c */
void	init_syms();		/* lookup.c */
void	init_verbs();		/* specials.c */
symbol	*lookup();		/* lookup.c */
void	main();			/* advgen.c */
#ifndef __STDC__
char	*malloc();		/* C library */
#endif
void	muldecl();		/* error.c */
void	muldef();		/* error.c */
void	negate();		/* code.c */
char	*noun_name();		/* vocab.c */
void	novarwarnings();	/* code.c */
bool	parse();		/* advgram.y */
void	printcode();		/* code.c */
void	printitems();		/* items.c */
void	printmsgs();		/* msgs.c */
void	printrooms();		/* rooms.c */
void	printspecials();	/* specials.c */
void	printvocab();		/* vocab.c */
char	*strrchr();		/* C library */
void	set_dirs();		/* specials.c */
void	set_drop_list();	/* specials.c */
void	set_get_list();		/* specials.c */
void	set_go_list();		/* specials.c */
void	set_greeting();		/* specials.c */
void	set_hell();		/* specials.c */
void	setinfile();		/* error.c */
void	setinput();		/* advlex.l */
void	set_inventory();	/* specials.c */
void	set_lamp();		/* specials.c */
void	set_llife();		/* specials.c */
void	set_lwarn();		/* specials.c */
void	set_nouns();		/* vocab.c */
void	setprogname();		/* error.c */
void	set_start();		/* specials.c */
void	set_treasury();		/* specials.c */
void	set_verbs();		/* vocab.c */
void	set_wsize();		/* specials.c */
char	*strcpy();		/* C library */
char	*strncpy();		/* C library */
char	*strsave();		/* str.c */
int	typecheck();		/* code.c */
void	typeerror();		/* error.c */
void	usage();		/* error.c */
void	warning();		/* error.c */
